/**
 * The main UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: MainWindow.h 1884 2024-11-25 09:10:00Z hazim.rujhan $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <queue>
#include <set>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProcess>
#include <QString>
#include <QTimer>

#include "AudioManager.h"
#include "Call.h"
#include "CallButton.h"
#include "Contact.h"
#include "Dgna.h"
#include "FlowLayout.h"
#include "GisWindow.h"
#ifdef INCIDENT
#include "Incident.h"
#endif
#include "Login.h"
#include "Logger.h"
#include "MsgSp.h"
#include "PalLock.h"
#include "Poi.h"
#include "Report.h"
#include "Resources.h"
#include "ResourceSelector.h"
#include "Sds.h"
#include "SettingsUi.h"
#include "ServerSession.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    /**
     * Callback function for received server messages.
     *
     * @param[in] obj MainWindow object, owner of the callback function.
     * @param[in] msg The message.
     */
    static void serverCallback(void *obj, MsgSp *msg);

signals:
    void serverMsg(MsgSp *msg);

public slots:
    /**
     * Handles an outgoing call request.
     *
     * @param[in] calledType The called party type - ResourceData::eType.
     * @param[in] ssi        The called party.
     * @param[in] doStart    true to start the call.
     */
    void onNewCall(int calledType, int ssi, bool doStart = false);

    /**
     * Handles an outgoing broadcast call request.
     */
    void onNewBroadcastCall();

    /**
     * Creates a Call Button for a connected incoming individual call.
     *
     * @param[in] ssi    The calling SSI.
     * @param[in] callId The call ID.
     */
    void onCallIncomingConnected(int ssi, int callId);

    /**
     * Deactivates the previous active CallWindow. An active CallWindow is the
     * only one using the microphone input.
     *
     * @param[in] cw The previous active CallWindow.
     */
    void onActiveOutCall(CallWindow *cw);

    /**
     * Disables all CallWindow audio output except the specified active one.
     *
     * @param[in] callId    The active call ID.
     * @param[in] callParty The called/calling party ID for a dispatcher call.
     */
    void onActiveInCall(int callId, int callParty);

    /**
     * Deletes a cancelled Call Window.
     *
     * @param[in] ssi The called SSI.
     */
    void onCallCancel(int ssi);

    /**
     * Ends a dispatcher call.
     *
     * @param[in] callParty The called or calling party ID.
     */
    void onCallRelease(int callParty);

    /**
     * Deletes a timed out call button (with call ID) or incoming call window
     * (with calling party ID).
     *
     * @param[in] callId       The call ID.
     * @param[in] callingParty The calling party ID. Used only if callId is 0.
     */
    void onCallTimeout(int callId, int callingParty);

    /**
     * Shows the Login dialog.
     *
     * @param[in] ok true if Settings input confirmed, false if cancelled.
     */
    void onSettingsFinished(bool ok);

private slots:
    /**
     * Applies color theme to UI components.
     */
    void onSetTheme();

    /**
     * Cancels login.
     */
    void onLoginCancel();

    /**
     * Processes any server message.
     *
     * @param[in] msg The server message.
     */
    void onServerMsg(MsgSp *msg);

    /**
     * Performs an action.
     *
     * @param[in] actType The action type.
     * @param[in] idType  The ID type - ResourceData::eType.
     * @param[in] ids     Space-separated IDs.
     */
    void onStartAction(int actType, int idType, const ResourceData::IdsT &ids);

    /**
     * Exits application.
     */
    void exitApp();

    /**
     * Shows and handles context menu to stop monitoring an ISSI/GSSI.
     *
     * @param[in] pos The mouse position.
     */
    void showMonContextMenu(const QPoint &pos);

    /**
     * Destroys Updater, and exits the application if necessary.
     *
     * @param[in] doExit true to exit the application.
     */
    void onUpdaterFinished(bool doExit);

protected:
    /**
     * Filters events of the watched object.
     *
     * @param[in] obj   The watched object.
     * @param[in] event The event.
     * @return true to stop the event being handled further.
     */
    bool eventFilter(QObject *obj, QEvent *event);

private:
    typedef std::map<int, CallButton *> CallMapT;
    typedef std::map<int, CallWindow *> CallWindowMapT;

    Ui::MainWindow     *ui;
    int                 mLoginFailCount;
    Logger             *mLogger;
    ServerSession      *mSession;
    SettingsUi         *mSettingsUi;
    GisWindow          *mGisWindow;
#ifdef INCIDENT
    Incident           *mIncident;
#endif
    Login              *mLogin;
    Report             *mReport;
    Resources          *mResources;
    Dgna               *mDgna;
    ResourceSelector   *mResourceSelector;
    Call               *mCall;
    Sds                *mSds;
    FlowLayout         *mIndFlowLayout;
    FlowLayout         *mGrpFlowLayout;
    AudioManager       *mAudioMgr;
    QLabel             *mStatusLbl;
    QLabel             *mStatusIconLbl;
    QLabel             *mStatusIssiLbl;
    QProcess           *mProc;          //for on-screen keyboard
    Poi                *mPoi;
    QString             mUserName;
    CallMapT            mCallMap;       //indexed by call ID
    CallWindowMapT      mCallWindowMap; //indexed by called/calling SSI
    QTimer              mMsgTimer;
    std::queue<MsgSp *> mMsgQueue;
    PalLock::LockT      mMsgQueueLock;  //guards mMsgQueue access
    std::map<int, QMdiSubWindow *> mMdiSubs;

    /**
     * Adds an MDI subwindow.
     *
     * @param[in] index  The subwindow index.
     * @param[in] widget The widget for the subwindow.
     */
    void mdiAdd(int index, QWidget *widget);

    /**
     * Activates an MDI subwindow.
     *
     * @param[in] index The subwindow index.
     */
    void mdiActivate(int index);

    /**
     * Gets the MDI subwindow activator button.
     *
     * @param[in] index The subwindow index.
     */
    QToolButton *getMdiBtn(int index);

    /**
     * Initializes other modules and updates the connection status.
     *
     * @param[in] stmNwk      true in STM-network mode.
     * @param[in] voipFailure true if VOIP session creation failed.
     * @param[in] issi        The server ISSI.
     * @param[in] helpDeskNum The Help Desk phone number, or empty string if
     *                        none.
     * @param[in] org         The organization name.
     */
    void onLoginSuccess(bool               stmNwk,
                        bool               voipFailure,
                        const std::string &issi,
                        const std::string &helpDeskNum,
                        const std::string &org);

    /**
     * Handles login failure. Exits after failure count reaches limit.
     *
     * @param[in] msg The server message containing failure details.
     */
    void onLoginFailure(MsgSp *msg);

    /**
     * Triggers data load if necessary, and shows this window if currently
     * hidden.
     *
     * @param[in] isSubsDataReady true if the subscriber data are ready.
     */
    void onReady(bool isSubsDataReady);

    /**
     * Connects common signals to/from a call window.
     *
     * @param[in] cw        The call window.
     * @param[in] activeOut true to connect activeOutCall signal.
     * @param[in] activeIn  true to connect activeInCall signal.
     */
    void connectCommonSignals(CallWindow *cw, bool activeOut, bool activeIn);

    /**
     * Shows the GPS monitoring selector dialog.
     *
     * @param[in] parent Parent widget.
     */
    void showGpsMon(QWidget *parent);

    /**
     * Gets a call button.
     *
     * @param[in] callId  The call ID.
     * @param[in] doErase true to remove the found call button from the map.
     * @return The call button, or 0 if not found.
     */
    CallButton *getCallButton(int callId, bool doErase = false);

    /**
     * Gets a call window.
     *
     * @param[in] callId The call ID.
     * @return The call window, or 0 if not found.
     */
    CallWindow *getCall(int callId);

    /**
     * Gets a call window that contains a particular call party.
     *
     * @param[in] ssi              The called/calling SSI.
     * @param[in] checkCallButtons true to also search the call windows assigned
     *                             to call buttons.
     *                             false to search only call windows that do not
     *                             have an associated call button.
     * @param[in] voipId           The VOIP call ID.
     * @return The call window, or 0 if not found.
     */
    CallWindow *getCallWindow(int                ssi,
                              bool               checkCallButtons,
                              const std::string &voipId = "");

    /**
     * Removes and deletes all call windows and buttons, and ends the associated
     * calls.
     */
    void removeCallWindows();

    /**
     * Removes a call window.
     *
     * @param[in] ssi      The called/calling SSI.
     * @param[in] doDelete true to delete the call window.
     * @param[in] force    true to force deletion even if keepOpen() returns
     *                     true.
     * @return true if the call window is found, but not necessarily removed.
     */
    bool removeCallWindow(int ssi, bool doDelete, bool force = false);

    /**
     * Removes and deletes a call window.
     *
     * @param[in] callId The call ID.
     */
    void removeCallWindow(int callId);

    /**
     * Deletes a call button or a call window. This should be practically the
     * only function that does this.
     * This makes it easier to perform other required actions for such event.
     *
     * @param[in] btn           The call button.
     * @param[in] checkKeepOpen true to check the call window's 'keep open'
     *                          before deleting.
     * @param[in] cw            The call window. If 0, taken from btn.
     * @param[in] callParty     The associated call party. If 0, taken from cw.
     */
    void deleteCall(CallButton *btn,
                    bool        checkKeepOpen,
                    CallWindow *cw = 0,
                    int         callParty = 0);

    /**
     * Checks the call simplex-duplex message field.
     *
     * @param[in] msg The server message.
     * @return true for duplex call.
     */
    bool isCallDuplex(const MsgSp &msg);

    /**
     * Checks the E2EE call encryption flag message field.
     *
     * @param[in] msg The server message.
     * @return true for E2EE call.
     */
    bool isCallE2ee(const MsgSp &msg);

    /**
     * Checks the call hook message field.
     *
     * @param[in] msg The server message.
     * @return true for hook call.
     */
    bool isCallHook(const MsgSp &msg);

    /**
     * Checks the call ownership message field.
     *
     * @param[in] msg The server message.
     * @return true if have ownership.
     */
    bool isCallOwner(const MsgSp &msg);

    /**
     * Checks the call Tx-Request-Permission message field.
     *
     * @param[in] msg The server message.
     * @return true if permission granted.
     */
    bool isCallPttAllowed(const MsgSp &msg);

    /**
     * Checks the call Tx-Grant message field.
     *
     * @param[in] msg The server message.
     * @return true if have Tx-Granted.
     */
    bool isCallTxGranted(const MsgSp &msg);

    /**
     * Convenience function to add contacts through the other addContacts().
     *
     * @param[in] type      The ID type - ResourceData::eType.
     * @param[in] doMonitor true to start monitoring.
     * @param[in] ids       The ISSIs or GSSIs.
     * @param[in] tabName   The tab name, if any.
     */
    void addContacts(int                       type,
                     bool                      doMonitor,
                     const ResourceData::IdsT &ids,
                     const QString            &tabName = "");

    /**
     * Adds contacts.
     *
     * @param[in] type        ID type - ResourceData::eType.
     * @param[in] userDefined true if added by user.
     * @param[in] idMap       The IDs and their monitoring setting.
     * @param[in] tabName     The tab name, if any.
     */
    void addContacts(int                     type,
                     bool                    userDefined,
                     const Contact::MonMapT &idMap,
                     const QString          &tabName);

    /**
     * Turns on/off highlighting of monitored subscriber or group involved in a
     * call.
     *
     * @param[in] caller The calling party.
     * @param[in] called The called party.
     * @param[in] on     true to turn on.
     */
    void monHighlight(int caller, int called, bool on);

    /**
     * Validates IDs using ResourceData::validate(), and shows an error dialog
     * if there are invalid IDs.
     *
     * @param[in]     type The resource type - ResourceData::eType.
     * @param[in,out] ids  The IDs, filtered on return.
     * @return true if the filtered ids is not empty.
     */
    bool validate(int type, ResourceData::IdsT &ids);

    /**
     * Handles a possible unconfirmed group attachment.
     *
     * @param[in] issi The ISSI.
     * @param[in] gssi The GSSI.
     */
    void grpUncAttach(int issi, int gssi);

    /**
     * Removes terminals from GisWindow. Caller must have checked for non-zero
     * mGisWindow.
     *
     * @param[in] rmList  If ssiList given, true to remove the listed ISSIs,
     *                    false to remove all except the ISSIs (i.e. exclude
     *                    list). Otherwise true to remove all, false to remove
     *                    ISSIs not attached to monitored groups.
     * @param[in] ssiList Comma-separated ISSI ranges.
     */
    void rmvMapTerminals(bool rmList, const std::string &ssiList = "");

    /**
     * Handles close event from window close button.
     */
    void closeEvent(QCloseEvent *event);

    /**
     * Checks whether a drag and drop event with a particular ID type is
     * acceptable for a list view.
     *
     * @param[in] lv     The list view.
     * @param[in] idType The ID type - CmnTypes::eIdType.
     * @return true if allowed.
     */
    bool isDragEventAllowed(QListView *lv, int idType);

    /**
     * Handles outgoing call failure.
     *
     * @param[in] cw  The CallWindow.
     * @param[in] msg The CALL-RELEASE message.
     */
    void handleCallFailure(CallWindow *cw, const MsgSp *msg);

    /**
     * Performs logout, and optionally exits the application.
     *
     * @param[in] doExit true to exit.
     */
    void logout(bool doExit);

    /**
     * Initiates client application update process.
     *
     * @param[in] msg The VERSION-CLIENT message.
     */
    void doUpdate(MsgSp *msg);

    /**
     * Deletes the server session asynchronously, resets mSession to 0 and
     * clears message queue if necessary.
     *
     * @param[in] session  Server session.
     * @param[in] haveLock true if caller is holding mMsgQueueLock.
     */
    void deleteSession(ServerSession *session, bool haveLock = false);
};
//use std::string as a signal parameter
Q_DECLARE_METATYPE(std::string)
#endif //MAINWINDOW_H
