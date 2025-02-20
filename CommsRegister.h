/**
 * UI Communications Register module. Displays either a call history table, a
 * message table, or both.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: CommsRegister.h 1817 2024-02-22 07:30:31Z rosnin $
 * @author Zulzaidi Atan
 */
#ifndef COMMSREGISTER_H
#define COMMSREGISTER_H

#include <map>
#include <QFileInfo>
#include <QIcon>
#include <QObject>
#include <QWidget>

#include "Logger.h"
#include "MessageDialog.h"
#include "MsgSp.h"
#include "QtTableUtils.h"

namespace Ui {
class CommsRegister;
}

class CommsRegister : public QWidget
{
    Q_OBJECT
public:
    enum eType
    {
        TYPE_CALL, //call table only
        TYPE_MSG,  //message table only
        TYPE_ALL   //all tables
    };

    /**
     * Constructor.
     *
     * @param[in] logger The app logger.
     * @param[in] type   eType.
     * @param[in] parent Parent widget, if any.
     */
    CommsRegister(Logger *logger, int type, QWidget *parent = 0);

    ~CommsRegister();

    void setUser(const QString &name, int id = 0);

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Adds data for incoming mobile call answered by another dispatcher.
     *
     * @param[in] msg MON_DISCONNECT message.
     */
    void addCall(MsgSp *msg);

    /**
     * Gets a file icon for a filename. If image file, icon is a thumbnail.
     * Otherwise icon is system-provided.
     *
     * @param[in] filename The filename.
     * @return The icon.
     */
    static QIcon getIcon(const QString &filename);

signals:
    void floatOrDock(QDialog *dlg);
    void missedCall();
    void mmsDownload(MsgSp *msg);
    void msgSelected(int type, const QString &msg);
    void newComm(int type, const QString &from, const QString &to);
    void startAction(int actType, int idType, int id, const QString &msg = "");
    void unreadMessages(int count);

public slots:
    /**
     * Handles call data.
     *
     * @param[in] type         Call type - CmnTypes::eCallType.
     * @param[in] priority     Call priority.
     * @param[in] isDuplex     true if duplex.
     * @param[in] startTime    Call start time.
     * @param[in] duration     Call duration.
     * @param[in] callingParty Calling party ID.
     * @param[in] calledParty  Called party ID.
     * @param[in] failedCause  Failure reason.
     * @param[in] pttData      PTT data, if any.
     */
    void onNewCall(int                              type,
                   int                              priority,
                   bool                             isDuplex,
                   const QString                   &startTime,
                   const QString                   &duration,
                   const QString                   &callingParty,
                   const QString                   &calledParty,
                   const QString                   &failedCause,
                   const MessageDialog::TableDataT &pttData);

    /**
     * Handles incoming SDS, Status message or MMS - either adds to the table,
     * or updates an existing entry.
     *
     * @param[in] msg The server message.
     * @param[in] err MMS error message, if any.
     * @return false if error message is not to be shown to user (because a
     *         similar message has been shown). Meaningful for MMS_RPT only.
     */
    bool onNewMsg(MsgSp *msg, const QString &err = "");

    /**
     * Adds an outgoing SDS or Status message to the table.
     *
     * @param[in] msgType The message type - CmnTypes::eCommsType.
     * @param[in] idType  The recipient ID type.
     * @param[in] dstId   The recipient ID.
     * @param[in] msgId   The message ID.
     * @param[in] txt     The message text.
     */
    void onOutMsg(int            msgType,
                  int            idType,
                  int            dstId,
                  int            msgId,
                  const QString &txt);

    /**
     * Adds an outgoing MMS to the table.
     *
     * @param[in] msg  The message.
     * @param[in] path File attachment local path, if any.
     * @param[in] err  Error message, if any.
     */
    void onOutMms(MsgSp *msg, const QString &path = "", const QString &err = "");

    /**
     * Updates resource texts in the call/msg tables because of a change in the
     * resource display text option.
     *
     * @param[in] type The resource type - ResourceData::eType. TYPE_GROUP is
     *                 also for DGNA groups.
     */
    void onRscDspOptChanged(int type);

private:
    Ui::CommsRegister *ui;
    Logger            *mLogger;
    int                mType;
    int                mUserId;
    int                mUnreadCount;
    int                mMmsKey;
    QString            mUserName; //with dispatcher prefix
    QString            mMmsSaveDir;

    QtTableUtils::IntSetT mCallHideDirs;
    QtTableUtils::IntSetT mMsgHideTypes;
    QtTableUtils::IntSetT mMsgHideDirs;

    struct MmsData
    {
        MmsData(bool out, int st, QString f, QString p, qint64 sz, MsgSp *m = 0)
            : state(st), msg(m), fname(f), path(p)
        {
            if (sz < 1024)
                info = QString::number(sz).append("B");
            else if (sz < (1024 * 1024))
                info = QString::number(sz/1024.0, 'f', 1).append("kB");
            else
                info = QString::number(sz/(1024.0 * 1024.0), 'f', 1)
                       .append("MB");
            //add file path (without filename) only for outgoing
            if (out)
                info.append("\n").append(QFileInfo(p).path());
        }
        MmsData() {}

        //add string to info
        MmsData &addInfo(const QString &s)
        {
            if (!s.isEmpty())
                info.append("\n").append(s);
            return *this;
        }

        //add unique string to info
        bool addUniqueInfo(const QString &s)
        {
            if (info.split("\n").contains(s))
                return false;
            info.append("\n").append(s);
            return true;
        }

        //check whether info contains a string
        bool hasInfo(const QString &s)
        {
            return info.split("\n").contains(s);
        }

        //remove last info line - only when expected to contain error
        void removeLastInfo()
        {
            info.truncate(info.lastIndexOf('\n'));
        }

        int      state;   //file delivery state
        MsgSp   *msg = 0; //MMS_TRANSFER
        QString  fname;   //file name
        QString  path;    //local path
        QString  info;    //file size & local paths, mainly for tooltip
    };
    typedef std::map<int, MmsData> MmsFileMapT; //key = msg ID/ACK
    typedef std::map<int, MmsFileMapT> MmsMapT; //key = mMmsKey
    MmsMapT mMmsMap;

    //icons for MMS status in transit, key = eDelStat (certain values only)
    static std::map<int, QIcon> mMmsIconMap;

    /**
     * Gets an individual call type name for call history display.
     *
     * @param[in] isDuplex true if duplex.
     * @return The name.
     */
    QString getIndividualCallName(bool isDuplex) const;

    /**
     * Tries to update a received MMS message in the message table.
     * MMS_TRANSFER with a failure result indicates download start failure.
     * Otherwise it may be for a subsequent file attachment in a single send.
     * MMS_RPT indicates a finished download (success or failure).
     *
     * @param[in] msg The message.
     * @param[in] err Error message, if any.
     * @return true if entry updated.
     */
    bool mmsCheckRx(MsgSp *msg, const QString &err = "");

    /**
     * Tries to update a sent MMS message in the message table.
     * MMS_TRANSFER with a failure result indicates upload failure. Otherwise it
     * may be for a subsequent file attachment in a single send.
     * MMS_RPT is for delivery confirmation (success or failure).
     *
     * @param[in] msg  The message.
     * @param[in] err  Error message, if any.
     * @param[in] path File attachment local path, if any.
     * @return MMS_RPT for recipient delivery confirmation: false if status
     *         shows none-file-transfer error already seen in the table entry.
     *         Otherwise: true if entry updated.
     */
    bool mmsCheckTx(MsgSp         *msg,
                    const QString &err = "",
                    const QString &path = "");

    /**
     * Shows MMS attachments in a message dialog.
     *
     * @param[in] md       The message dialog.
     * @param[in] key      Data key for mMmsMap.
     * @param[in] outgoing true for outgoing message.
     */
    void mmsShowAtt(MessageDialog *md, int key, bool outgoing);

    /**
     * Gets the icon for an MMS delivery status in transit.
     * If the required icon is not yet stored in the lookup table, creates and
     * adds it to the table.
     *
     * @param[in] stat The delivery status - eDelStat.
     * @return The icon.
     */
    const QIcon &mmsStatIcon(int stat);

    /**
     * Checks either all MMS data or selected message table rows for at least
     * one unsaved received file attachment.
     *
     * @param[in] all true to check all. Otherwise only selected rows.
     * @return true if found.
     */
    bool mmsUnsavedAtt(bool all);

    /**
     * Deletes received MMS files.
     *
     * @param[in] all true to delete all. Otherwise only the ones no longer in
     *                the message table.
     */
    void mmsCleanup(bool all);

    /**
     * Sets message delivery status.
     *
     * @param[in] msg The server message.
     */
    void setDeliveryStatus(MsgSp *msg);

    /**
     * Stores type data and icon into a message table cell in the first row.
     *
     * @param[in] col  The table column.
     * @param[in] type The type - CmnTypes::eCommsType.
     */
    void setCellData(int col, int type);

    /**
     * Gets the type data from a message table cell that was stored using
     * setCellData().
     *
     * @param[in] row The table row.
     * @param[in] col The table column.
     * @return The stored data, or 0 if none found.
     */
    int getCellData(int row, int col);

    /**
     * Convenience function to create a QTableWidgetItem with left-top text
     * alignment.
     *
     * @param[in] txt The item text, if any.
     * @return The item.
     */
    QTableWidgetItem *newItem(const QString &txt = "");

    /**
     * Either updates the displayed unread messages counter here or emits the
     * signal to update it elsewhere.
     */
    void unreadCountChanged();

    /**
     * Gets a printable document for the call/message tables and shows print
     * preview or export dialog.
     *
     * @param[in] printType The print document type - Document::ePrintType.
     */
    void doPrint(int printType);
};
#endif //COMMSREGISTER_H
