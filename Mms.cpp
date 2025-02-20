/**
 * Qt implementation of MMS handler.
 * Uses MmsClient to send to server and download from a server URL.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: Mms.cpp 1513 2021-07-28 07:57:39Z rosnin $
 * @author Rosnin
 * @author Mohd Rozaimi
 */
#include <QFileInfo>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

#include "CmnTypes.h"
#include "QtUtils.h"
#include "Settings.h"
#include "Utils.h"
#include "Mms.h"

using namespace std;

static const string LOGPREFIX("Mms::");

Mms::Mms(Logger *logger, MmsClient *mmsClient) :
mMmsClient(mmsClient), mLogger(logger), mSession(0)
{
    setDownloadDir();
}

Mms::~Mms()
{
    delete mMmsClient;
    mMmsClient = 0; //for send() thread
    LOGGER_DEBUG(mLogger, LOGPREFIX << " Destroyed");
}

void Mms::setSession(ServerSession *ss)
{
    mSession = ss;
    if (ss != 0)
        mMmsClient->setUser(ss->getUsername(), ss->getServerAddress(false));
}

bool Mms::setDownloadDir()
{
    string d(Settings::instance().get<string>(Props::FLD_CFG_MMS_DOWNLOADDIR));
    if (!d.empty())
    {
        if (QDir(QString::fromStdString(d)).exists())
        {
            mMmsClient->setDownloadDir(d, true);
            return true;
        }
        LOGGER_WARNING(mLogger, LOGPREFIX << "User download directory " << d
                       << " no longer exists - using default");
    }
    mMmsClient->setDownloadDir(QStandardPaths::writableLocation(
                                           QStandardPaths::AppLocalDataLocation)
                                   .toStdString().append("/download/"),
                               false);
    return false;
}

QString Mms::send(MsgSp *msg, const QString &path)
{
    if (msg == 0)
    {
        assert("Bad param in Mms::send" == 0);
        return "Null msg";
    }
    assert(!path.isEmpty() || msg->hasField(MsgSp::Field::USER_DATA));
    if (!msg->hasField(MsgSp::Field::MSG_REF))
    {
        //add ref even if fails here, for CommsRegister
        msg->addField(MsgSp::Field::MSG_REF, Utils::getUniqueRef());
        if (msg->hasField(MsgSp::Field::USER_DATA))
            msg->setUserText();
    }
    QFile *f = 0;
    QString err;
    if (!path.isEmpty())
    {
        //add FILE_LIST in transmission format
        msg->addField(MsgSp::Field::FILE_LIST,
                  Utils::toHexString(QFileInfo(path).fileName().toStdString()));
        qint64 sz = 0;
        f = new QFile(path);
        if (!f->open(QFile::ReadOnly))
        {
            LOGGER_ERROR(mLogger, LOGPREFIX << "send: Failed to open file "
                         << path.toStdString());
            err.append(tr("Error opening file"));
        }
        else
        {
            sz = f->size();
            if (sz == 0)
            {
                LOGGER_ERROR(mLogger, LOGPREFIX << "send: Empty file "
                             << path.toStdString());
                err.append(tr("Empty file"));
            }
            else if (sz > MmsClient::MAXSIZE)
            {
                LOGGER_ERROR(mLogger, LOGPREFIX << "send: File '"
                             << path.toStdString() << "' size " << sz
                             << " bytes exceeds max " << MmsClient::MAXSIZE);
                err.append(tr("File too big - %1 bytes (max %2)")
                           .arg(sz).arg(MmsClient::MAXSIZE));
            }
        }
        msg->addField(MsgSp::Field::FILE_SIZE, sz);
    }
    if (err.isEmpty())
    {
        if (mSession->sendMsg(msg, false) <= 0)
        {
            err.append(tr("Error sending message"));
        }
        else if (f != 0)
        {
            int ctx = mMmsClient->txStart(msg, path.toStdString());
            if (ctx > 0)
            {
                //send in separate thread
                QTimer::singleShot(0, [this, f, ctx]
                {
                    QtConcurrent::run([this, f, ctx]
                    {
                        int r = 0;
                        int num = 2; //retries
                        QByteArray b;
                        do
                        {
                            r = 1;
                            while (!f->atEnd() && mMmsClient != 0)
                            {
                                b = f->read(2048);
                                if (b.isEmpty())
                                {
                                    if (r < 0)
                                        break;
                                    r = -1;
                                    continue; //retry read once
                                }
                                r = mMmsClient->txSend(ctx, b.size(), b.data());
                                if (r <= 0)
                                    break;
                            }
                            if (r > 0 || --num < 0 || !f->reset() ||
                                mMmsClient == 0 ||
                                mMmsClient->txRestart(ctx) <= 0)
                                break;
                        }
                        while (r <= 0); //condition here not really important
                        delete f;
                        if (mMmsClient != 0)
                            mMmsClient->txEnd(ctx, r);
                    });
                });
            }
            else if (ctx == MmsClient::ERR_CONNECT)
            {
                err.append(tr("Server connection failed"));
            }
            else
            {
                err.append(tr("Error sending file"));
            }
        }
    }
    if (!err.isEmpty() && f != 0)
    {
        delete f;
        err.prepend(tr("File '%1' upload error: ").arg(getFileList(msg)));
    }
    return err;
}

QString Mms::rcv(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in Mms::rcv" == 0);
        return "Null msg";
    }
    LOGGER_DEBUG(mLogger, LOGPREFIX << "rcv:\n" << *msg);
    QString err;
    if (msg->getType() == MsgSp::Type::MMS_RPT)
    {
        bool ok = msg->isResultSuccessful();
        if (msg->hasField(MsgSp::Field::ID))
        {
            //download finished callback from MmsClient
            int ctx = msg->getFieldInt(MsgSp::Field::ID);
            msg->removeField(MsgSp::Field::ID);
            mMmsClient->rxEnd(ctx);
            string path; //local path
            if (ok)
            {
                //see MmsClient::dlThread()
                path = msg->getFieldString(MsgSp::Field::USER_DEFINED_DATA_1);
                msg->removeField(MsgSp::Field::USER_DEFINED_DATA_1);
            }
            else
            {
                switch (msg->getFieldInt(MsgSp::Field::RESULT))
                {
                    case MsgSp::Value::RESULT_MMSERR_DOWNLOAD:
                        err.append(tr("MMS Error: "))
                           .append(tr("File '%1' download failed - retry later")
                                   .arg(getFileList(msg)));
                        break;
                    case MsgSp::Value::RESULT_MMSERR_DOWNLOAD_PERM:
                        err.append(tr("MMS Error: "))
                           .append(tr("File '%1' no longer available")
                                   .arg(getFileList(msg)));
                        break;
                    default:
                        break; //do nothing
                }
            }
            //USER_DEFINED_DATA_2 means user retry - no more failure response
            if (mSession != 0 &&
                (ok || !msg->hasField(MsgSp::Field::USER_DEFINED_DATA_2)))
            {
                //remove internal field before sending & restore after -
                //see MmsClient::rxMsg()
                string s(msg->getFieldString(MsgSp::Field::STORAGE));
                if (!s.empty())
                    msg->removeField(MsgSp::Field::STORAGE);
                mSession->sendMsg(msg, false);
                if (!s.empty())
                    msg->addField(MsgSp::Field::STORAGE, s);
                //change to local path if successful
                if (ok)
                    msg->addField(MsgSp::Field::FILE_PATH, path);
            }
        }
        else if (!ok)
        {
            if (!msg->hasMmsError())
            {
                //not file transfer error
                err.append(tr("MMS error to %1: ")
                           .arg(msg->getFieldInt(MsgSp::Field::CALLING_PARTY)))
                   .append(QString::fromStdString(
                               msg->getFieldValueString(MsgSp::Field::RESULT)));
            }
            else if (msg->hasField(MsgSp::Field::CALLING_PARTY))
            {
                err.append(tr("File '%1' delivery error to %2: ")
                           .arg(getFileList(msg))
                           .arg(msg->getFieldInt(MsgSp::Field::CALLING_PARTY)))
                   .append(tr("Recipient download error"));
            }
            else
            {
                //server upload failure
                err.append(tr("File '%1' upload error: ").arg(getFileList(msg)))
                   .append(QString::fromStdString(
                               msg->getFieldValueString(MsgSp::Field::RESULT)));
            }
        }
        //else presence of CALLING_PARTY means recipient download success,
        //otherwise server upload success - do nothing here in both cases
        return err;
    }
    assert(msg->getType() == MsgSp::Type::MMS_TRANSFER);
    if (!msg->hasField(MsgSp::Field::CALLING_PARTY))
    {
        //upload failure from MmsClient
        err = tr("File '%1' upload error: ").arg(getFileList(msg));
        switch (msg->getFieldInt(MsgSp::Field::RESULT))
        {
            case MsgSp::Value::RESULT_MMSERR_CONNECT:
                err.append(tr("Server connection failed"));
                break;
            case MsgSp::Value::RESULT_MMSERR_FILE:
                err.append(tr("Error opening file"));
                break;
            default:
                err.append(tr("Error sending file"));
                break;
        }
        return err;
    }
    //incoming
    LOGGER_DEBUG(mLogger, LOGPREFIX << "rcv: From "
                 << msg->getFieldInt(MsgSp::Field::CALLING_PARTY) << " \""
                 << msg->getUserText() << "\" "
                 << msg->getFieldString(MsgSp::Field::FILE_PATH));
    return download(msg, false);
}

QString Mms::download(MsgSp *msg, bool retry)
{
    if (msg == 0 || msg->getType() != MsgSp::Type::MMS_TRANSFER)
    {
        assert("Bad param in Mms::download" == 0);
        return "Invalid msg";
    }
    QString err;
    string pStr;
    auto *resp = mMmsClient->rxMsg(msg, pStr); //resp is MMS_RPT
    if (pStr.empty())
    {
        //no file attachment - can only happen in call from rcv()
        mSession->sendMsg(resp);
        return err;
    }
    //if download started, resp contains context ID
    if (!resp->hasField(MsgSp::Field::ID))
    {
        if (mMmsClient->isUsrDownloadDir() && !setDownloadDir())
        {
            //user download dir no longer valid & changed to default - retry
            mMmsClient->rxMsg(msg, pStr, resp);
        }
        if (!resp->hasField(MsgSp::Field::ID))
        {
            err = tr("File '%1' download failed, error writing file: %2")
                  .arg(getFileList(msg)).arg(QString::fromStdString(pStr));
            resp->addField(MsgSp::Field::RESULT,
                           MsgSp::Value::RESULT_MMSERR_DOWNLOAD);
            if (retry)
            {
                emit dlFailed(resp);
            }
            else
            {
                msg->addField(MsgSp::Field::RESULT,
                              MsgSp::Value::RESULT_MMSERR_DOWNLOAD);
                mSession->sendMsg(resp);
            }
            return err;
        }
    }
    if (retry)
        resp->addField(MsgSp::Field::USER_DEFINED_DATA_2, "1"); //for rcv()
    return err;
}

inline QString Mms::getFileList(const MsgSp *msg)
{
    assert(msg != 0);
    return QtUtils::fromHexUnicode(msg->getFieldString(MsgSp::Field::FILE_LIST));
}
