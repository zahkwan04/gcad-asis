/**
 * Platform-indepedent class to manage MMS messaging with server.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2022. All Rights Reserved.
 *
 * @file
 * @version $Id: MmsClient.h 1623 2022-07-21 03:52:29Z rosnin $
 * @author Rosnin
 * @author Mohd Rozaimi
 */
#ifndef MMSCLIENT_H
#define MMSCLIENT_H

#include <cstdio>   //FILE, file I/O functions, remove()
#include <map>
#include <string>

#ifdef MSG_AES
#include "Aes.h"
#endif
#include "Logger.h"
#include "MsgSp.h"
#include "PalThread.h"
#include "TcpSocket.h"

class MmsClient
{
public:
    static const int MAXSIZE = 1024 * 1024 * 1024; //max file 1GB

    //callback signature for MMS message
#ifdef MOBILE
    typedef void (*CallbackFn)(MsgSp *msg);
    typedef void (*StrCallbackFn)(const std::string &str);
#else
    typedef void (*CallbackFn)(void *obj, MsgSp *msg);
#endif

    //errors - must be negative
    enum eErr
    {
        ERR_CONNECT = -100, //server connection failure
        ERR_SEND,           //send failure
        ERR_INVALID         //any other
    };

#ifdef MOBILE
    /**
     * Constructor.
     *
     * @param[in] cbFn    Callback function for MMS message.
     * @param[in] strCbFn Callback function to pass a log string.
     */
    MmsClient(CallbackFn cbFn, StrCallbackFn strCbFn);

#else
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] cbObj  Callback function owner.
     * @param[in] cbFn   Callback function for MMS message.
     */
    MmsClient(Logger *logger, void *cbObj, CallbackFn cbFn);
#endif //MOBILE

    ~MmsClient();

    /**
     * Sets the server port for file upload.
     *
     * @param[in] port Port number.
     */
    void setSvrPort(int port) { mSvrPort = port; }

    /**
     * Sets the user ID and server IP address for file upload.
     *
     * @param[in] id User ID.
     * @param[in] ip Server IP.
     */
    void setUser(const std::string &id, const std::string &ip)
    {
        mUserId = id;
        mSvrIp = ip;
    }

#ifdef MOBILE
    /**
     * Starts file download.
     *
     * @param[in] msg  The MMS_RPT response message.
     * @param[in] fp   The opened file for writing.
     * @param[in] path The file local path.
     * @return The download context ID to use with rxEnd().
     */
    int rxStart(MsgSp *msg, FILE *fp, const std::string &path);

#else
    /**
     * Sets the download directory.
     *
     * @param[in] dir The directory.
     * @param[in] usr true if set by user. false if default.
     */
    void setDownloadDir(const std::string &dir, bool usr);

    bool isUsrDownloadDir() { return mUsrDlDir; }

    /**
     * Handles a received MMS_TRANSFER message by creating MMS_RPT response.
     * If message has file attachment, creates it with a unique path in the
     * download directory, and starts a new download thread.
     *
     * @param[in]  msg  The message.
     * @param[out] path The created download file path if applicable,
     *                  otherwise unmodified.
     * @param[in]  resp The previous response on retry, otherwise 0.
     * @return The new response, or previous response on retry.
     */
    MsgSp *rxMsg(const MsgSp *msg, std::string &path, MsgSp *resp = 0);
#endif //MOBILE

    /**
     * Ends a file download process by stopping the thread and erasing the
     * context.
     *
     * @param[in] ctx The download context ID.
     */
    void rxEnd(int ctx);

    /**
     * Performs file download in a separate thread.
     *
     * @param[in] ctx The download context ID.
     */
    void dlThread(int ctx);

    /**
     * Starts file upload by calling txConnect().
     * Must be followed by txSend() and txEnd() calls.
     *
     * @param[in] msg  The MMS_TRANSFER message.
     * @param[in] path The file local path.
     * @return The positive context ID for subsequent txSend() and txEnd(), or
     *         eErr on failure.
     */
    int txStart(const MsgSp *msg, const std::string &path);

    /**
     * Restarts file upload previously started by txStart() but failed.
     *
     * @param[in] ctx Context ID from txStart().
     * @return Return value from txConnect(), or 0 for invalid context ID..
     */
    int txRestart(int ctx);

    /**
     * Sends file data as part of an on-going upload.
     *
     * @param[in] ctx  Context ID from txStart().
     * @param[in] num  The number of data bytes.
     * @param[in] data The data to send.
     * @return Positive value if successful.
     *         0 for invalid context ID.
     *         Negative for error to use with txEnd().
     */
    int txSend(int ctx, int num, char *data);

    /**
     * Finishes a file upload and deletes the context.
     *
     * @param[in] ctx Context ID from txStart().
     * @param[in] res The last result from txSend(). Positive if successful.
     */
    void txEnd(int ctx, int res);

    /**
     * Starts a data cipher process.
     *
     * @param[in] msg MMS_TRANSFER message for upload, MMS_RPT for download.
     * @param[in] ctx Unique context ID which will also be used for subsequent
     *                cipherRun() and cipherEnd().
     */
    void cipherStart(const MsgSp *msg, int ctx);

    /**
     * Performs data cipher.
     *
     * @param[in]     fwd  true to encipher, false to decipher.
     * @param[in]     ctx  Context ID in cipherStart().
     * @param[in,out] data The data.
     * @return true for valid output. false for insufficient input.
     */
    bool cipherRun(bool fwd, int ctx, std::string &data);

    /**
     * Ends a data cipher process, deleting the context.
     *
     * @param[in] ctx Context ID in cipherStart().
     */
    void cipherEnd(int ctx);

private:
    bool           mStopped;
    int            mSvrPort;  //server port for upload
    std::string    mUserId;
    std::string    mSvrIp;    //server IP for upload
    std::string    mDlDir;    //download directory
#ifdef MOBILE
    CallbackFn     mCbFn;     //callback function
    StrCallbackFn  mStrCbFn;
#else
    bool           mUsrDlDir; //true if user set mDlDir
    Logger        *mLogger;
    void          *mCbObj;    //callback function owner
    CallbackFn     mCbFn;     //callback function
#endif //MOBILE

    //context data for file receiving
    struct RxCtx
    {
        RxCtx(const std::string &p, PalThread::ThreadT t, MsgSp *m, FILE *f) :
            fPath(p), tId(t), msg(m), fp(f) {}
        RxCtx() {}

        std::string         fPath;   //local path
        PalThread::ThreadT  tId = 0;
        MsgSp              *msg = 0; //MMS_RPT
        FILE               *fp  = 0;
    };
    std::map<int, RxCtx> mRxCtxMap; //key is context ID

    //context data for file sending
    struct TxCtx
    {
        TxCtx(MsgSp *m, TcpSocket *s) : msg(m), sock(s) {}
        TxCtx() {}

        Utils::TimepointT  startTp = Utils::getTimepoint();
        MsgSp             *msg     = 0; //MMS_TRANSFER
        TcpSocket         *sock    = 0; //server connection
    };
    std::map<int, TxCtx> mTxCtxMap; //key is context ID

    //context data for file encryption (send & receive)
    struct CipherCtx
    {
#ifdef MSG_AES
        CipherCtx(int ref, int size, const std::string &k)
        {
            //calculate checksum & add to every key char for correlation
            char ch = ref;
            for (auto &c : k)
            {
                ch ^= c;
            }
            for (auto &c : k)
            {
                key.append(1, c + ch);
            }
            Aes::validateKey(key);
            pendingSz = size; //only for tx
        }
        CipherCtx() {}

        //for tx restart
        void reset(MsgSp *msg)
        {
            data.clear();
            pendingSz = msg->getFieldInt(MsgSp::Field::FILE_SIZE);
        }

        std::string key;
        std::string data; //unprocessed data
        size_t      pendingSz;
#else
        CipherCtx(int ref, const std::string &k)
        {
            //calculate checksum & add to every key char for correlation
            ch = 0;
            for (auto &c: k)
            {
                ch ^= c;
            }
            for (auto &c: k)
            {
                key.append(1, c + ch);
            }
            ch = static_cast<char>(ref);
        }
        CipherCtx() {}

        //for retry
        void reset(MsgSp *msg)
        {
            idx = 0;
            time_t ref = 0;
            msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
            ch = static_cast<char>(ref);
        }

        std::string key;
        int         idx = 0; //running key char index
        char        ch  = 0; //last relevant char during operation
#endif //MSG_AES
    };
    std::map<int, CipherCtx> mCipherMap; //key is context ID

    /**
     * Gets a new unique context ID for file upload/download.
     *
     * @return The positive context ID.
     */
    int getNewContext();

    /**
     * Connects to server and sends header data for file upload.
     *
     * @param[in] msg  The MMS_TRANSFER message.
     * @param[in] sock The socket for connection. Deleted on failure.
     * @param[in] path The file local path.
     * @return Positive number of header bytes sent if successful, otherwise
     *         eErr.
     */
    int txConnect(const MsgSp *msg, TcpSocket *sock, const std::string &path);
};
#endif //MMSCLIENT_H
