/**
 * UI SDS, MMS and Status Message module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Sds.h 1862 2024-06-04 06:33:19Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#ifndef SDS_H
#define SDS_H

#include <map>
#include <QIcon>
#include <QStringList>
#include <QWidget>

#include "CommsRegister.h"
#include "Logger.h"
#include "Mms.h"
#include "MmsClient.h"
#include "MsgSp.h"
#include "ResourceData.h"
#include "ResourceSelector.h"
#include "ServerSession.h"

namespace Ui {
class Sds;
}

class Sds : public QWidget
{
    Q_OBJECT

public:
    static const int SDSMAXLEN = 140;
    static const int MMSMAXLEN = 1000;

    /**
     * Constructor.
     *
     * @param[in] logger      The app logger.
     * @param[in] rscSelector Resource Selector object.
     * @param[in] commsReg    Message register. Takes ownership.
     * @param[in] mmsClient   MMS server interface. Takes ownership.
     * @param[in] parent      Parent widget, if any.
     */
    explicit Sds(Logger           *logger,
                 ResourceSelector *rscSelector,
                 CommsRegister    *commsReg,
                 MmsClient        *mmsClient,
                 QWidget          *parent = 0);

    ~Sds();

    /**
     * Loads ResourceSelector object.
     */
    void activate();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    CommsRegister *getCommsRegister() { return mCommsReg; }

    void setSession(ServerSession *session, const QString &userName = "")
    {
        mSession = session;
        mMms->setSession(session);
        mCommsReg->setUser(userName);
    }

    void setMmsSvrPort(int port) { mMms->setSvrPort(port); }

    /**
     * Gets the message template from the template combobox.
     *
     * @return The template strings, or empty string if no template.
     */
    QString getTemplate();

    /**
     * Loads the predefined Status Message table data.
     */
    void loadData();

    /**
     * Updates the Status Message data.
     *
     * @param[in] msg The server message.
     */
    void updateStsData(MsgSp *msg);

    /**
     * Sets individual recipients and sends them messages if provided.
     *
     * @param[in] idType The recipient type - CmnTypes::eIdType.
     * @param[in] ids    The recipients.
     * @param[in] msgs   The messages. Empty to let user write one.
     * @param[in] txt    The complete unsplit text for MMS if applicable.
     */
    void sendMsg(int                       idType,
                 const ResourceData::IdsT &ids,
                 const QStringList        &msgs,
                 const QString            &txt = "");

    /**
     * Handles a received MMS message.
     *
     * @param[in] msg The message. Takes ownership.
     */
    void mmsRcv(MsgSp *msg);

    /**
     * Sets the selected input tab based on the given action type.
     *
     * @param[in] actType CmnTypes::ACTIONTYPE_SDS or ACTIONTYPE_STATUS.
     */
    void startAction(int actType);

    /**
     * Prepares messages for sending by combining where possible, based on the
     * length and size limit.
     * Input is expected to be short strings, with only the last one possibly
     * long and truncated to the limit.
     *
     * @param[in]     prefix For each message.
     * @param[in,out] msgs   In:  Message strings to prepare.
     *                       Out: Messages ready to send.
     */
    static void prepMsgs(const QString &prefix, QStringList &msgs);

public slots:
    /**
     * Enables the Send button if the recipient single-selection or
     * multi-selection is valid and message text is not empty.
     * Disables it otherwise.
     *
     * @param isValidSingle true if single-selection is valid.
     * @param isValidMulti  true if multi-selection is valid.
     */
    void onSelectionChanged(bool isValidSingle, bool isValidMulti);

    /**
     * Notifies MMS module on download directory change.
     */
    void onMmsDownloadDirChanged() { mMms->setDownloadDir(); }

    /**
     * Retries MMS download.
     *
     * @param msg MMS_TRANSFER message.
     */
    void onMmsDownload(MsgSp *msg) { mMms->download(msg, true); }

protected:
    //override
    bool eventFilter(QObject *, QEvent *event);

private:
    Ui::Sds              *ui;
    CommsRegister        *mCommsReg;
    Mms                  *mMms;
    int                   mRecState;  //recipient selection state
    int                   mMsgState;  //msg input state
    QStringList           mTemplate;  //SDS template strings
    std::map<int, QIcon>  mStsIconMap;
    //following objects are owned by another module
    Logger               *mLogger;
    ServerSession        *mSession;
    ResourceSelector     *mResourceSelector;

    struct StsData
    {
        int     type;
        QString text;
    };
    std::map<int, StsData> mStsDataMap; //key is status code

    /**
     * Refreshes the Status Message data based on server data.
     */
    void refreshStsData();

    /**
     * Adds data to the Status Message table.
     *
     * @param[in] row  The row.
     * @param[in] type Status type.
     * @param[in] code Status code. Skipped if negative.
     * @param[in] text Status text in hexadecimal UTF8 Unicode.
     */
    void addStsTableData(int row, int type, int code, const QString &text);

    /**
     * Adds/modifies/deletes a Status Message list item.
     *
     * @param[in] doMenu true to update the Status Message button menu.
     * @param[in] code   Status code. Skipped if negative - special case to
     *                   update the menu only.
     * @param[in] type   Status type - add/modify only.
     * @param[in] text   Status text in hexadecimal UTF8 Unicode - add/modify
     *                   only.
     */
    void setStsItem(bool           doMenu,
                    int            code,
                    int            type = -1,
                    const QString &text = "");

    /**
     * Sets a Status Message type icon based on its description. If no
     * description, sets to default icon.
     *
     * @param[in] type Status type.
     * @param[in] desc The type description in hexadecimal UTF8 Unicode.
     */
    void setStsIcon(int type, const std::string &desc);

    /**
     * Gets a Status Message type icon.
     *
     * @param[in] type Status type.
     * @return The icon.
     */
    QIcon &getStsIcon(int type);

    /**
     * Sets or clears Status Message mode.
     *
     * @param[in] on true to set.
     */
    void setStsMode(bool on);

    /**
     * Gets the selected Status Message code value, if any.
     *
     * @return The status code if in Status Message mode, or -1 otherwise.
     */
    int getStsCode();

    /**
     * Sets text to the SDS/Status message box.
     *
     * @param[in] txt          The text.
     * @param[in] clearStsMode true to clear Status Message mode.
     */
    void setMsgText(const QString &txt, bool clearStsMode = true);

    /**
     * Calculates remaining message characters and shows it only if below
     * threshold. Truncates MMS text beyond limit.
     * Finally calls setSdsSendEnabled().
     */
    void onMsgEditTextChanged();

    /**
     * Adds unique MMS file attachments.
     *
     * @param[in] files The filenames.
     */
    void mmsAddFiles(const QStringList &files);

    /**
     * Enables or disables the Send button based on input status and recipient
     * selection.
     */
    void setSendEnabled();

    /**
     * Handles Send button click.
     *
     * @param[in] type The message type - CmnTypes::eCommsType.
     *                 Omit to let function determine based on UI.
     */
    void onSendButtonClick(int type = -1);

    /**
     * Sends MMS.
     *
     * @param[in] sel The non-empty recipients selection.
     */
    void mmsSend(const ResourceSelector::SelectionsT &sel);

    /**
     * Updates the recent messages or attachments list (history menu) by adding
     * or moving one to the top.
     *
     * @param[in] menu  The history menu.
     * @param[in] item  Item to update.
     * @param[in] isMsg true for message, false for attachment.
     */
    void updateHistMenu(QMenu *menu, const QString &item, bool isMsg);

    /**
     * Gets the byte-count of a Unicode string, which may contain multi-byte
     * characters.
     *
     * @param[in] str The string.
     * @return The size.
     */
    static int getSize(const QString &str) { return str.toUtf8().size(); }
};
#endif //SDS_H
