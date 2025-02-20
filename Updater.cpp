/**
 * App Auto update implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Updater.cpp 1899 2025-01-22 05:42:07Z rosnin $
 * @author Rosnin Mustaffa
 */
#include <assert.h>

#include "Md5Digest.h"
#include "ui_Updater.h"
#include "Version.h"
#include "Updater.h"

using namespace std;

static const string  LOGPREFIX("Updater:: ");
//download directory
static const QString DIR(QStandardPaths::writableLocation(
                                                 QStandardPaths::TempLocation) +
                         "/");

bool     Updater::sIsCreated(false);
bool     Updater::sLogout(false);
QString  Updater::sSkipVersion;
Updater *Updater::sInstance(0);
Logger  *Updater::sLogger(0);

#ifdef _WIN32
PalLock::LockT Updater::sSingletonLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT Updater::sSingletonLock(QMutex::Recursive);
#else
PalLock::LockT Updater::sSingletonLock = PTHREAD_MUTEX_INITIALIZER;
#endif

Updater &Updater::instance(QWidget *parent)
{
    //a modified Double Checked Locking Pattern for thread-safe Singleton, lock
    //obtained only before/during creation
    if (!sIsCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!sIsCreated) //thread-safety double check
        {
            sInstance = new Updater(parent);
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                sIsCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

void Updater::setLogout(bool loggedOut)
{
    sLogout = loggedOut;
    if (loggedOut)
        sSkipVersion.clear(); //reset
}

void Updater::destroy()
{
    PalLock::take(&sSingletonLock);
    if (sIsCreated)
    {
        sIsCreated = false;
        sInstance->deleteLater();
        sInstance = 0;
    }
    PalLock::release(&sSingletonLock);
}

void Updater::updateApp(bool foreground, MsgSp *msg)
{
    if (msg == 0 || msg->getType() != MsgSp::Type::VERSION_CLIENT)
    {
        assert("Bad param in Updater::updateApp" == 0);
        return;
    }
    QString version(QString::fromStdString(
                                   msg->getFieldString(MsgSp::Field::VERSION)));
    if (version == Version::APP_VERSION || version == sSkipVersion)
    {
        //version unchanged or already skipped earlier
        LOGGER_DEBUG(sLogger, LOGPREFIX << "updateApp: Skipping v"
                     << version.toStdString());
        return;
    }
    if (!start(msg, "*.exe"))
    {
        if (foreground)
            QMessageBox::critical(this, tr("Update Error"),
                                  tr("Error getting installer."));
        emit finished(foreground);
        return;
    }
    LOGGER_DEBUG(sLogger, LOGPREFIX << "updateApp: "
                 << ((foreground)? "foreground": "background") << " v"
                 << version.toStdString());
    mForeground = foreground;
    if (mForeground)
    {
        ui->lblMsg->setText(tr("Please wait while download is in progress."));
        ui->lblDownload->setText(tr("Downloading..."));
        ui->cancelButton->setText(tr("Cancel"));
        ui->okButton->setEnabled(false);
        ui->progressBar->show();
        show();
    }
    else
    {
        sSkipVersion = version; //to be cleared if user clicks okButton
        //prepare message for use after download completed
        ui->lblMsg->setText(tr("A new version %1 is ready. "
                               "This is version %2.\n")
                               .arg(version, Version::APP_VERSION) +
                            tr("Click '%1' to begin installation.")
                               .arg(ui->okButton->text()));
        ui->lblDownload->setVisible(false);
        ui->cancelButton->setText(tr("Not now"));
    }
    startDownload();
}

void Updater::onFinished()
{
    bool isMainFile = (mFile != 0 && mFile->fileName() == mMainLocalPath);
    QNetworkReply::NetworkError err = mReply->error();
    if (err == QNetworkReply::NoError)
    {
        LOGGER_INFO(sLogger, LOGPREFIX << "onFinished:"
                    << mFile->fileName().toStdString());
        //file may not be open if already downloaded earlier
        if (mFile->isOpen())
        {
            mFile->flush();
            mFile->close();
        }
        if (isMainFile)
        {
            int res = validateMain(true);
            if (res < 0)
            {
                emit finished(mForeground);
                return;
            }
            if (res > 0)
                --mFileIdx; //retry
        }
        delete mFile;
        mFile = 0;
        mTimer.stop();
        mReply->deleteLater();
        mReply = 0;
    }
    else
    {
        if (err == QNetworkReply::OperationCanceledError)
        {
            LOGGER_INFO(sLogger, LOGPREFIX << "onFinished:"
                        << "OperationCanceledError\nFile: "
                        << ((mFile != 0)? mFile->fileName().toStdString(): ""));
        }
        else
        {
            LOGGER_ERROR(sLogger, LOGPREFIX << "onFinished:"
                         << mReply->errorString().toStdString() << "\nFile: "
                         << mReply->url().toString().toStdString());
            sSkipVersion.clear();
            if (mForeground)
                QMessageBox::critical(this, tr("Update Error"),
                                      tr("Download failed."));
        }
        if (mFile != 0)
        {
            if (mFile->isOpen())
            {
                mFile->close();
                mFile->remove();
            }
            mFile->deleteLater();
            mFile = 0;
        }
        mTimer.stop();
        mReply->deleteLater();
        mReply = 0;
        if (isMainFile)
        {
            emit finished(mForeground);
            return;
        }
        //otherwise continue with next file, if any
    }
    if (++mFileIdx < mFileList.size())
    {
        startDownload();
    }
    else if (mForeground)
    {
        ui->lblMsg->setText(tr("Click '%1' to begin installation.")
                            .arg(ui->okButton->text()));
        ui->lblDownload->setText(tr("Download completed."));
        ui->okButton->setEnabled(true);
    }
    else if (!sLogout)
    {
        show();
    }
}

Updater::Updater(QWidget *parent) :
QDialog(parent), ui(new Ui::Updater), mFileIdx(0), mFile(0), mReply(0)
{
    mManager = new QNetworkAccessManager(this);
    mTimer.setSingleShot(true);
    mTimer.setInterval(90000); //timeout in 90 seconds
    connect(&mTimer, &QTimer::timeout, this,
            [this]
            {
                LOGGER_ERROR(sLogger, LOGPREFIX << "Download timeout for "
                             << mFile->fileName().toStdString());
                mReply->abort();
            });
    ui->setupUi(this);
    ui->lblMsg->setWordWrap(true);
    ui->okButton->setDefault(true);
    ui->cancelButton->setAutoDefault(false);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setModal(false);
    connect(ui->okButton, &QPushButton::clicked, this,
            [this]
            {
                //start upgrade process
                sSkipVersion.clear();
                hide();
                runUpdate();
            });
    connect(ui->cancelButton, &QPushButton::clicked, this,
            [this]
            {
                //cancel upgrade process
                if (mReply != 0)
                    mReply->abort();
                else
                    emit finished(mForeground);
            });
}

Updater::~Updater()
{
    mTimer.stop();
    if (mReply != 0)  //app exit while download still in progress
        mReply->deleteLater();
    if (mFile != 0)
    {
        if (mFile->isOpen())
        {
            mFile->close();
            mFile->remove();
        }
        delete mFile;
    }
    delete mManager;
    delete ui;
}

bool Updater::start(MsgSp *msg, const QString &extRegExp)
{
    //only one update at a time
    if (!mFileList.isEmpty())
    {
        LOGGER_WARNING(sLogger, LOGPREFIX << "start: "
                       "Update already in progress.");
        return false;
    }
    QString files(QString::fromStdString(
                                 msg->getFieldString(MsgSp::Field::FILE_LIST)));
    mFileList = files.split(MsgSp::Value::LIST_DELIMITER);
    //check if main file exists
    int idx = mFileList.indexOf(QRegExp(extRegExp, Qt::CaseInsensitive,
                                        QRegExp::Wildcard));
    if (idx < 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "start:" << "Missing main '"
                     << extRegExp.toStdString() << "' file in list: "
                     << files.toStdString());
        return false;
    }
    mMainLocalPath = DIR + mFileList.at(idx);
    mMainDownloadCount = 4;  //maximum attempts
    mFileIdx = 0;
    mFilePath = QString::fromStdString(
                                  msg->getFieldString(MsgSp::Field::FILE_PATH));
    if (!mFilePath.endsWith('/'))
        mFilePath.append('/');
    mMainChksum = msg->getFieldString(MsgSp::Field::CHECKSUM);
    return true;
}

void Updater::startDownload()
{
    //set url file path
    QUrl url(mFilePath + mFileList.at(mFileIdx));
    //set a file for the download
    mFile = new QFile(DIR + mFileList.at(mFileIdx));
    if (mFile->exists() &&
        (mFile->fileName() != mMainLocalPath || validateMain(false) == 0))
    {
        //ensure mReply->error() returns NoError
        mReply = mManager->head(QNetworkRequest(url));
        ui->progressBar->setValue(ui->progressBar->maximum());
        onFinished();
        return;
    }
    if (!mFile->open(QIODevice::WriteOnly))
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "startDownload: Error writing file "
                     << mFile->fileName().toStdString() << ": "
                     << mFile->errorString().toStdString());
        if (mForeground)
            QMessageBox::critical(this, tr("Update Error"),
                                  tr("Unable to save file %1: %2.")
                                      .arg(mFile->fileName(),
                                           mFile->errorString()));
        delete mFile;
        mFile = 0;
        emit finished(mForeground);
        return;
    }
    LOGGER_INFO(sLogger, LOGPREFIX << "startDownload:"
                << mFile->fileName().toStdString());
    QNetworkRequest req(url);
    req.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    //post a request to obtain the contents of the target
    mReply = mManager->get(req);
    mReply->ignoreSslErrors();
    connect(mReply, &QNetworkReply::readyRead, this,
            [this]
            {
                mTimer.start();
                //write the received data into file
                if (mFile->isOpen())
                    mFile->write(mReply->readAll());
            });
    //for progressBar indicator
    connect(mReply, &QNetworkReply::downloadProgress, this,
            [this](qint64 bytesRead, qint64 totalBytes)
            {
                if (totalBytes != 0)
                {
                    ui->progressBar->setMaximum(totalBytes);
                    ui->progressBar->setValue(bytesRead);
                }
            });
    connect(mReply, SIGNAL(finished()), SLOT(onFinished()));
}

void Updater::runUpdate()
{
    bool res = QProcess::startDetached(mMainLocalPath);
    if (res)
    {
        LOGGER_INFO(sLogger, LOGPREFIX << "runUpdate: Installer running: "
                    << mMainLocalPath.toStdString());
    }
    else
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "runUpdate: Failed to start installer "
                     << mMainLocalPath.toStdString()
                     << " - deleting possibly corrupted file");
        QFile::remove(mMainLocalPath);
        QMessageBox::critical(this, tr("Update Error"),
                              tr("Failed to run installer. ") +
                              tr("Will retry on next login."));
    }
    emit finished(res); //exit only after running installer
}

int Updater::validateMain(bool decrementCount)
{
    if (mFile->size() > 15000000) //basic size check
    {
        if (mMainChksum.empty() ||
            mMainChksum == md5DigestFile(mMainLocalPath.toStdString()))
            return 0;
        LOGGER_ERROR(sLogger, LOGPREFIX << "validateMain: Checksum failed for "
                     << mMainLocalPath.toStdString());
    }
    if (decrementCount && --mMainDownloadCount <= 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "validateMain: "
                     << mFile->fileName().toStdString()
                     << " max failed attempts - aborting");
        mFile->remove();
        delete mFile;
        mFile = 0;
        mReply->deleteLater();
        mReply = 0;
        sSkipVersion.clear();
        if (mForeground)
            QMessageBox::critical(this, tr("Update Error"),
                                  tr("Failed to download installer. ") +
                                      tr("Will retry on next login."));
        return -1;
    }
    LOGGER_ERROR(sLogger, LOGPREFIX << "validateMain: "
                 << mFile->fileName().toStdString() << " size=" << mFile->size()
                 << " - possibly corrupted and deleted - retry "
                 << mMainDownloadCount);
    mFile->remove();
    return 1;
}
