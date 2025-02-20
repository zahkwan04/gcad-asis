/**
 * Platform-independent implementation of MMS interface to server.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: MmsClient.cpp 1897 2025-01-16 02:11:11Z zulzaidi $
 * @author Rosnin
 * @author Mohd Rozaimi
 */
#ifndef MOBILE
#include <cstdio>   //std::remove()
#endif
#include <time.h>   //time_t, time()

#include "PalThread.h"
#include "Utils.h"
#include "MmsClient.h"

#include <assert.h>

using namespace std;

struct DlThreadParam
{
    DlThreadParam(MmsClient *o, int c) : obj(o), ctx(c) {}

    MmsClient *obj; //this object
    int        ctx; //download context ID
};

static void *startDlThread(void *arg)
{
    auto *p = static_cast<DlThreadParam *>(arg);
    MmsClient *obj = p->obj;
    int ctx = p->ctx;
    delete p;
    obj->dlThread(ctx);
    return 0;
}

static const string LOGPREFIX("MmsClient::");

#ifdef MOBILE
#define MSGCB(arg) mCbFn(arg)
static ostringstream sOss;
#define LOGX(lvl, arg)                  \
        do                              \
        {                               \
            LOGGER_##lvl(mLogger, arg); \
            sOss << arg;                \
            mStrCbFn(sOss.str());       \
            sOss.str("");               \
        }                               \
        while (0)

MmsClient::MmsClient(Logger *logger, CallbackFn cbFn, StrCallbackFn strCbFn) :
mStopped(false), mSvrPort(0), mLogger(logger), mCbFn(cbFn), mStrCbFn(strCbFn)
{
}

#else //MOBILE
#define MSGCB(arg) mCbFn(mCbObj, arg)
#define LOGX(lvl, arg) LOGGER_##lvl(mLogger, arg)

MmsClient::MmsClient(Logger *logger, void *cbObj, CallbackFn cbFn) :
mStopped(false), mSvrPort(0), mUsrDlDir(false), mLogger(logger), mCbObj(cbObj),
mCbFn(cbFn)
{
    if (logger == 0 || cbObj == 0)
    {
        assert("Bad param in MmsClient::MmsClient" == NULL);
        return;
    }
}
#endif //MOBILE

MmsClient::~MmsClient()
{
    mStopped = true;
    for (auto &it : mTxCtxMap)
    {
        delete it.second.msg;
        delete it.second.sock;
    }
    //stop ongoing download
    for (auto &it : mRxCtxMap)
    {
        delete it.second.msg;
        it.second.msg = 0;
        PalThread::stop(it.second.tId);
        fclose(it.second.fp);
#ifndef MOBILE
        remove(it.second.fPath.c_str()); //delete local file
#endif
    }
    LOGGER_DEBUG(mLogger, LOGPREFIX << " Destroyed");
}

#ifdef MOBILE
int MmsClient::rxStart(MsgSp *msg, FILE *fp, const std::string &path)
{
    int ctx = getNewContext();
    LOGX(DEBUG, LOGPREFIX << "rxStart [" << ctx << "]");
    PalThread::ThreadT tid;
    PalThread::start(&tid, startDlThread, new DlThreadParam(this, ctx));
    mRxCtxMap[ctx] = RxCtx(path, tid, msg, fp);
    msg->addField(MsgSp::Field::ID, ctx); //internal use
    return ctx;
}

#else
void MmsClient::setDownloadDir(const string &dir, bool usr)
{
    mDlDir = dir;
    if (!dir.empty() && dir.back() != '/')
        mDlDir.append("/");
    mUsrDlDir = usr;
    LOGGER_DEBUG(mLogger, LOGPREFIX << " Download dir " << mDlDir);
}

MsgSp *MmsClient::rxMsg(const MsgSp *msg, string &path, MsgSp *resp)
{
    if (msg == 0 || msg->getType() != MsgSp::Type::MMS_TRANSFER)
    {
        assert("Bad param in MmsClient::rxMsg" == 0);
        return 0;
    }
    bool retry = (resp != 0);
    if (!retry)
    {
        resp = new MsgSp(*msg);
        resp->setType(MsgSp::Type::MMS_RPT)
             .addField(MsgSp::Field::MSG_ACK, msg->getMsgId())
             .addField(MsgSp::Field::CALLED_PARTY,
                       msg->getFieldString(MsgSp::Field::CALLING_PARTY))
             .addField(MsgSp::Field::CALLED_PARTY_TYPE,
                       msg->getFieldString(MsgSp::Field::CALLING_PARTY_TYPE))
             .removeField(MsgSp::Field::CALLING_PARTY)
             .removeField(MsgSp::Field::CALLING_PARTY_TYPE)
             .removeField(MsgSp::Field::USER_DATA);
        if (msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
            MsgSp::Value::IDENTITY_TYPE_GSSI)
            resp->addField(MsgSp::Field::GSSI,
                           msg->getFieldString(MsgSp::Field::CALLED_PARTY));
    }
    string url(msg->getFieldString(MsgSp::Field::FILE_PATH));
    if (url.empty())
        return resp; //no attachment
    if (mUsrDlDir)
    {
        //create unique path with original filename and sender ID
        path.assign(mDlDir)
            .append(Utils::fromHexString(
                                 msg->getFieldString(MsgSp::Field::FILE_LIST)));
        Utils::makeUniqueFilepath(path,
                              msg->getFieldString(MsgSp::Field::CALLING_PARTY));
        //indicate usr dir to upper level - internal use only
        resp->addField(MsgSp::Field::STORAGE, "1");
    }
    else
    {
        //create unique path with filename "<calledParty>-<url filename>" -
        //server has ensured a unique <url filename>
        path.assign(mDlDir)
            .append(msg->getFieldString(MsgSp::Field::CALLED_PARTY))
            .append("-").append(url.erase(0, url.find_last_of("/") + 1));
        if (retry)
            resp->removeField(MsgSp::Field::STORAGE);
    }
    FILE *fp = fopen(path.c_str(), "wb");
    if (fp == NULL)
    {
        LOGGER_ERROR(mLogger, LOGPREFIX << "rxMsg: Failed to open file " << path
                     << " for writing");
    }
    else
    {
        int ctx = getNewContext();
        LOGGER_DEBUG(mLogger, LOGPREFIX << "rxMsg: Download [" << ctx << "]");
        PalThread::ThreadT tid;
        PalThread::start(&tid, startDlThread, new DlThreadParam(this, ctx));
        mRxCtxMap[ctx] = RxCtx(path, tid, resp, fp);
        resp->addField(MsgSp::Field::ID, ctx); //internal use
    }
    return resp;
}
#endif //MOBILE

void MmsClient::rxEnd(int ctx)
{
    if (mRxCtxMap.count(ctx) == 0)
    {
        LOGX(ERROR, LOGPREFIX << "rxEnd: Invalid ctx " << ctx);
    }
    else
    {
        LOGX(DEBUG, LOGPREFIX << "rxEnd [" << ctx << "]");
        PalThread::stop(mRxCtxMap[ctx].tId);
        mRxCtxMap.erase(ctx);
    }
}

void MmsClient::dlThread(int ctx)
{
    ostringstream oss;
#ifndef MOBILE
    oss << LOGPREFIX;
#endif
    oss << "dlThread[" << ctx << "] ";
    string logPref(oss.str());
    oss.clear();
    oss.str("");
    //ensure context data ready from rxStart()/rxMsg()
    int retry = 10; //maximum tries
    while (mRxCtxMap.count(ctx) == 0 || mRxCtxMap[ctx].msg == 0)
    {
        if (--retry == 0)
        {
            LOGX(ERROR, logPref << "Missing context data in map, aborting");
            return;
        }
        PalThread::msleep(5);
    }
    auto &d(mRxCtxMap[ctx]);
    string url(d.msg->getFieldString(MsgSp::Field::FILE_PATH));
    string ip(url);
    ip.erase(0, ip.find("//") + 2);  //delete "http://" or "https://"
    auto pos = ip.find('/');
    string path(ip.substr(pos));     //must start with /
    ip.erase(pos);                   //ip:port
    pos = ip.find(':');
    string port(ip.substr(pos + 1)); //port
    ip.erase(pos);                   //ip
    auto *sock = new TcpSocket(ip, Utils::fromString<int>(port),
                               (url.compare(0, 5, "https") == 0));
    Utils::TimepointT startTp = Utils::getTimepoint();

    const int  MAX_TIMEOUT = 5; //successive timeouts to declare failure
    int        res = -1;        //result
    int        timeoutCount;
    int        bytesRcvd;
    int        fSize = d.msg->getFieldInt(MsgSp::Field::FILE_SIZE);
    string     str;
    string     hdr;
    char       buf[65535];
    retry = 5;
    while (retry-- > 0)
    {
        if (sock->connect() != 0)
        {
            if (retry == 0)
            {
                LOGX(ERROR, logPref << url
                     << " server connection failed, aborting");
                res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD;
                break;
            }
            LOGX(ERROR, logPref << url
                 << " server connection failed, retrying...");
            PalThread::msleep(100);
            continue;
        }
        res = -1; //any negative
        //send request
        oss << "GET " << path << " HTTP/1.1\r\nHost: " << ip
            << "\r\nConnection: close\r\n\r\n";
        sock->send(oss.str());
        LOGX(DEBUG, logPref << oss.str());
        oss.clear();
        oss.str("");

        timeoutCount = MAX_TIMEOUT;
        fSize = d.msg->getFieldInt(MsgSp::Field::FILE_SIZE); //down counter
        str.clear();
        hdr.clear();
        rewind(d.fp);
        while (res < 0)
        {
            //use short timeout to allow quick interrupt for thread termination
            bytesRcvd = sock->recv(buf, sizeof(buf), 5);
            if (bytesRcvd == TcpSocket::ERR_TIMEOUT)
            {
                if (--timeoutCount == 0)
                {
                    LOGX(ERROR, logPref << "Receive timeout");
                    res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD;
                }
                continue;
            }
            timeoutCount = MAX_TIMEOUT; //reset
            if (bytesRcvd <= 0)
            {
                if (Socket::isDisconnectedError(-bytesRcvd))
                {
                    LOGX(ERROR, logPref << "Error " << bytesRcvd
                         << Socket::getErrorStr(-bytesRcvd)
                         << "- server disconnected");
                    res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD;
                }
                else
                {
                    LOGX(DEBUG, logPref << "Error " << bytesRcvd
                         << Socket::getErrorStr(-bytesRcvd));
                }
                continue;
            }
            str.append(buf, bytesRcvd);
            if (hdr.empty())
            {
                pos = str.find("\r\n\r\n");
                if (pos == string::npos)
                {
                    LOGX(ERROR, logPref << "Bad HTTP header");
                    res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD;
                    continue;
                }
                hdr = str.substr(0, pos);
                str.erase(0, pos + 4);
                LOGGER_DEBUG(mLogger, logPref << "HTTP header ("
                             << (hdr.size() + 4) << ")\n" << hdr);
                //hdr start when file not available: HTTP/1.1 404 Not Found
                if (hdr.find("404 Not Found") != string::npos)
                {
                    res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD_PERM;
                    retry = 0;
                    break;
                }
                cipherStart(d.msg, ctx);
                if (str.empty())
                    continue;
                bytesRcvd = str.size();
            }
            pos = str.size(); //reuse pos
            if (cipherRun(false, ctx, str))
            {
                pos = str.size();
                if (fwrite(str.data(), 1, pos, d.fp) == pos)
                {
                    fSize -= pos;
                    if (fSize <= 0)
                    {
                        res = 0;
                        retry = 0;
                        break;
                    }
                    str.clear();
                }
                else
                {
                    LOGX(ERROR, logPref << "Failed to write to file "
                         << d.fPath);
                    res = MsgSp::Value::RESULT_MMSERR_DOWNLOAD;
                    break;
                }
            }
        } //while (res < 0)
    } //while (retry-- > 0)
    cipherEnd(ctx);
    delete sock;
    if (d.msg != 0) //not stopped by destructor
    {
        fclose(d.fp);
        if (res == 0)
        {
            LOGX(VERBOSE, logPref << "Received from "
                 << d.msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                 << " " << url << " to " << d.fPath << " ("
                 << Utils::getTransferStats(startTp,
                                    d.msg->getFieldInt(MsgSp::Field::FILE_SIZE))
                 << ")");
            //add local path
            d.msg->addField(MsgSp::Field::USER_DEFINED_DATA_1, d.fPath);
        }
        else
        {
#ifndef MOBILE
            remove(d.fPath.c_str()); //delete local file
#endif
            d.msg->addField(MsgSp::Field::RESULT, res);
        }
        MSGCB(d.msg);
    }
    LOGGER_DEBUG(mLogger, logPref << "finished");
}

int MmsClient::txStart(const MsgSp *msg, const string &path)
{
    if (msg == 0)
    {
        assert("Bad param in MmsClient::txStart" == 0);
        return ERR_INVALID;
    }
    assert(msg->getType() == MsgSp::Type::MMS_TRANSFER);
    auto *sock = new TcpSocket(mSvrIp, mSvrPort);
    int res = txConnect(msg, sock, path);
    if (res <= 0)
        return res;
    //keep a copy of msg with added FILE_PATH just for log
    MsgSp *m = new MsgSp(*msg);
    m->addField(MsgSp::Field::FILE_PATH, path);
    int ctx = getNewContext();
    cipherStart(m, ctx);
    mTxCtxMap[ctx] = TxCtx(m, sock);
    LOGGER_DEBUG(mLogger, LOGPREFIX << "txStart [" << ctx << "]");
    return ctx;
}

int MmsClient::txRestart(int ctx)
{
    if (mTxCtxMap.count(ctx) == 0)
    {
        LOGX(ERROR, LOGPREFIX << "txRestart: Invalid ctx " << ctx);
        return 0;
    }
    auto &cData(mTxCtxMap[ctx]);
    mCipherMap[ctx].reset(cData.msg);
    PalThread::msleep(200); //wait before reconnecting
    LOGGER_DEBUG(mLogger, LOGPREFIX << "txRestart " << ctx);
    //reuse ctx for result
    ctx = txConnect(cData.msg, cData.sock,
                    cData.msg->getFieldString(MsgSp::Field::FILE_PATH));
    if (ctx <= 0)
        cData.sock = 0; //already deleted
    return ctx;
}

int MmsClient::txSend(int ctx, int num, char *data)
{
    if (data == 0)
    {
        assert("Bad param in MmsClient::txSend" == 0);
        return 0;
    }
    if (mTxCtxMap.count(ctx) == 0)
    {
        LOGX(ERROR, LOGPREFIX << "txSend: Invalid ctx " << ctx);
        return 0;
    }
    auto &cData(mTxCtxMap[ctx]);
    string s(data, num);
    if (!cipherRun(true, ctx, s))
        return 1; //any positive for caller to keep going
    int err = 0;
    int res = 0;
    while (!s.empty() && !mStopped)
    {
        res = cData.sock->send(s.data(), s.size());
        if (res > 0)
        {
            s.erase(0, res);
            err = 0;
            continue;
        }
        //many consecutive WouldBlock errors indicate failure on server which
        //stops it from receiving data
        if (!Socket::isWouldBlockError(-res) || ++err >= 10)
            return res;
        PalThread::msleep(100); //wait and retry
    }
    return res;
}

void MmsClient::txEnd(int ctx, int res)
{
    if (mTxCtxMap.count(ctx) == 0)
    {
        LOGX(ERROR, LOGPREFIX << "txEnd: Invalid ctx " << ctx);
        return;
    }
    auto &cData(mTxCtxMap[ctx]);
    delete cData.sock;
    cipherEnd(ctx);
    if (res > 0)
    {
        int64_t sz;
        cData.msg->getFieldVal(MsgSp::Field::FILE_SIZE, sz);
        LOGX(VERBOSE, LOGPREFIX << " Sent file [" << ctx << "] '"
             << cData.msg->getFieldString(MsgSp::Field::FILE_PATH)
             << "' (" << Utils::getTransferStats(cData.startTp, sz) << ")");
        delete cData.msg;
    }
    else
    {
        LOGX(ERROR, LOGPREFIX << "txEnd " << ctx << " '"
             << cData.msg->getFieldString(MsgSp::Field::FILE_PATH)
             << "' failed sending, " << res << Socket::getErrorStr(-res));
        cData.msg->addField(MsgSp::Field::RESULT,
                            MsgSp::Value::RESULT_MMSERR_UPLOAD);
        LOGGER_VERBOSE(mLogger, LOGPREFIX << " Callback\n" << *cData.msg);
        MSGCB(cData.msg);
    }
    mTxCtxMap.erase(ctx);
}

void MmsClient::cipherStart(const MsgSp *msg, int ctx)
{
    assert(msg != 0);
    time_t ref = 0;
    msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
    //base = SCAD code birth time+review
    string k(Utils::scramble(ref, "20130624084430R3r844",
                             msg->getFieldString(MsgSp::Field::FILE_LIST)));
    k.append(msg->getFieldString(MsgSp::Field::MSG_REF));
    mCipherMap[ctx] = CipherCtx(ref,
#ifdef MSG_AES
                                msg->getFieldInt(MsgSp::Field::FILE_SIZE),
#endif
                                k);
}

bool MmsClient::cipherRun(bool fwd, int ctx, string &data)
{
    if (mCipherMap.count(ctx) == 0)
    {
        assert("Bad param in MmsClient::cipherRun" == NULL);
        return false;
    }
    auto &cd(mCipherMap[ctx]);
#ifdef MSG_AES
    //plain text may have an incomplete final block,
    //cipher text must have a complete final block
    auto &d(cd.data);
    if (fwd)
        cd.pendingSz -= data.size();
    size_t sz = d.append(data).size();
    int n = Aes::getSizeExcess(sz); //bytes beyond last block
    if (n < 0 && (!fwd || cd.pendingSz > 0))
        return false; //incomplete block
    string s;
    if (n != 0 && (!fwd || cd.pendingSz > 0))
    {
        //process all blocks except the incomplete last one
        sz -= n; //bytes to process
        s = d.substr(sz); //extra bytes to save for later
        d.erase(sz);
    }
    if (fwd)
        data = Aes::encrypt(d, cd.key);
    else
        data = Aes::decrypt(d, cd.key);
    d = s; //save unprocessed data, if any
    return true;
#else //MSG_AES
    auto &key = cd.key;
    auto &ch = cd.ch;
    string::const_iterator itk = key.cbegin() + cd.idx;
    if (fwd)
    {
        //process each data byte with each key char
        for (auto &c : data)
        {
            //use previous output for sequential correlation
            ch = (c + *itk) ^ ch;
            c = ch;
            if (++itk == key.cend())
                itk = key.cbegin();
        }
    }
    else
    {
        char tmp;
        for (auto &c : data)
        {
            tmp = c;
            c = (c ^ ch) - *itk;
            ch = tmp;
            if (++itk == key.cend())
                itk = key.cbegin();
        }
    }
    //cast to prevent compiler warning
    cd.idx = static_cast<int>(itk - key.cbegin());
    return true;
#endif //MSG_AES
}

void MmsClient::cipherEnd(int ctx)
{
    mCipherMap.erase(ctx);
}

int MmsClient::getNewContext()
{
    static int ctx = 0; //running context id
    if (++ctx <= 0)
        ctx = 1;
    return ctx;
}

int MmsClient::txConnect(const MsgSp *msg, TcpSocket *sock, const string &path)
{
    int res = 0;
    int n = 5; //connection retries
    do
    {
        if (sock->connect() == 0)
        {
            res = sock->setNonblocking(); //can do this only after connect()
            break;
        }
        //allow time for server to be ready to accept connection
        PalThread::msleep(200);
    }
    while (!mStopped && --n > 0);
    if (mStopped || n == 0 || res != 0)
    {
        if (!mStopped)
        {
            if (n == 0)
                LOGX(ERROR, LOGPREFIX << "txConnect '" << path
                     << "' failed to connect to server "
                     << sock->getRemoteAddrStr());
            else
                //cannot continue with blocking socket, because in case of
                //failure on server, socket send() may block forever
                LOGX(ERROR, LOGPREFIX << "txConnect '" << path
                     << "' socket setNonblocking() failed, " << res
                     << Socket::getErrorStr(-res));
            MsgSp *m = new MsgSp(*msg);
            m->addField(MsgSp::Field::RESULT,
                        MsgSp::Value::RESULT_MMSERR_CONNECT);
            LOGGER_VERBOSE(mLogger, LOGPREFIX << " Callback\n" << *m);
            MSGCB(m);
        }
        delete sock;
        return ERR_CONNECT;
    }
    //must start with raw header "<clientID> <msgID>MsgSp::Value::ENDL"
    string hdr(mUserId);
    hdr.append(" ").append(msg->getFieldString(MsgSp::Field::MSG_ID))
       .append(1, MsgSp::Value::ENDL);
    //cast just to silence compiler check
    res = sock->send(hdr.data(), static_cast<int>(hdr.size()));
    if (res <= 0)
    {
        delete sock;
        return ERR_SEND;
    }
    LOGX(DEBUG, LOGPREFIX << "txConnect '" << path << "' connected to server "
         << sock->getRemoteAddrStr() << ", hdr='" << hdr.erase(hdr.size() - 1)
         << "'");
    return res;
}
