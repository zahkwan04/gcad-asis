/**
 * Qt-specific MMS handler class.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: Mms.h 1513 2021-07-28 07:57:39Z rosnin $
 * @author Rosnin
 * @author Mohd Rozaimi
 */
#ifndef MMS_H
#define MMS_H

#include <QObject>

#include "Logger.h"
#include "MmsClient.h"
#include "MsgSp.h"
#include "ServerSession.h"

class Mms : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger    App logger.
     * @param[in] mmsClient MMS interface to server. Takes ownership.
     */
    Mms(Logger *logger, MmsClient *mmsClient);

    ~Mms();

    /**
     * Sets the current server session.
     *
     * @param[in] ss The server session. 0 when disconnected.
     */
    void setSession(ServerSession *ss);

    /**
     * Sets the server MMS port.
     *
     * @param[in] port The port number.
     */
    void setSvrPort(int port) { mMmsClient->setSvrPort(port); }

    /**
     * Sets the download directory in MmsClient - either user directory from
     * settings, or default directory.
     *
     * @return true if user directory.
     */
    bool setDownloadDir();

    /**
     * Sends MMS.
     * If message does not have MSG_REF, generates a unique value and adds to
     * message, and sets the user text if present.
     * A message that already has MSG_REF is a subsequent message in a
     * multi-file send.
     * If message has file attachment, gets the file size and adds to message.
     * Sends the message, and then sends the file attachment via MmsClient.
     *
     * @param[in,out] msg  The MMS message.
     * @param[in]     path The file attachment local path, if any.
     * @return Error message, if any. Otherwise empty string.
     */
    QString send(MsgSp *msg, const QString &path = "");

    /**
     * Handles a received MMS_TRANSFER/RPT, either incoming from server or
     * callback from MmsClient.
     * Passes incoming MMS_TRANSFER to download().
     * For MMS_RPT, returns error string for failure indicated by the message.
     *
     * @param[in] msg The message.
     * @return Empty string if successful, otherwise error message.
     */
    QString rcv(MsgSp *msg);

    /**
     * Handles incoming MMS_TRANSFER - either upon receipt or to retry a
     * previously failed attachment download.
     * If no attachment, just sends response to server.
     * Otherwise starts download through MmsClient.
     * If download start fails, adds failure result to the message, and sends
     * response to server if not in retry.
     *
     * @param[in] msg   MMS_TRANSFER message.
     * @param[in] retry true for retry.
     * @return Empty string if successful, otherwise error message.
     */
    QString download(MsgSp *msg, bool retry);

    /**
     * Convenience function to get the message FILE_LIST converted from
     * transmission format.
     *
     * @param[in] msg The message.
     * @return The converted string.
     */
    QString getFileList(const MsgSp *msg);

signals:
    void dlFailed(MsgSp *msg); //download failed to start

private:
    MmsClient     *mMmsClient;
    //following objects are not owned
    Logger        *mLogger;
    ServerSession *mSession;
};
#endif //MMS_H
