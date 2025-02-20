/**
 * UI Quick Contact module for fast access communications.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Contact.h 1821 2024-03-04 00:13:29Z rosnin $
 * @author Rosnin
 */
#ifndef CONTACT_H
#define CONTACT_H

#include <map>
#include <set>
#include <QPoint>
#include <QScrollArea>
#include <QTabWidget>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "CallWindow.h"
#include "Draggable.h"
#include "Logger.h"
#include "ResourceData.h"
#include "Resources.h"
#include "ServerSession.h"

namespace Ui {
class Contact;
}

class Contact : public QWidget, public Draggable
{
    Q_OBJECT

public:
    typedef std::set<Contact *> ContactsT;
    typedef std::map<int, bool> MonMapT;     //key=ID, value=monitor

    //constructor is private
    ~Contact();

    int getId() const { return mId; }

    int getType() const { return mType; }

    QString getName() const { return mName; }

    bool isGrp() const { return mIsGrp; }

    bool hasCall(CallWindow *call) const { return (call == mCall); }

    /**
     * Checks whether this contact is selected.
     *
     * @return true if selected.
     */
    bool selected() const;

    /**
     * Checks whether this contact is system-created.
     *
     * @return true if system-created.
     */
    bool sysDefined() const;

    /**
     * Checks whether this contact is either user-created, or system-created but
     * marked for saving by user.
     *
     * @return true if either condition fulfilled.
     */
    bool userDefined() const;

    /**
     * Gets the parent tab. This is either the parent at creation, or the
     * current container tab.
     *
     * @return The tab.
     */
    QScrollArea *getTab() const;

    /**
     * Sets the monitored state.
     *
     * @param[in] on true if monitored.
     */
    void setMonitored(bool on);

    /**
     * Gets the monitored state.
     *
     * @return true if monitored.
     */
    bool monitored() const;

    /**
     * Sets the notes text.
     *
     * @param[in] txt      The text.
     * @param[in] isByUser true if set by user.
     * @return false if set by user and the text length exceeds the maximum
     *         length.
     */
    bool setNotes(QString &txt, bool isByUser = false);

    /**
     * Sets or clears the active indicator.
     */
    void setGrpActive();

    /**
     * Handles a new call.
     *
     * @param[in] call The call window.
     */
    void callSetup(CallWindow *call);

    /**
     * Handles call connection.
     *
     * @param[in] call The call window. Mainly for incoming call answered by
     *                 another client.
     */
    void callConnect(CallWindow *call = 0);

    /**
     * Handles group call inclusion.
     */
    void callInclude();

    /**
     * Updates the PTT button appearance based on the call window.
     */
    void pttUpdate();

    /**
     * Handles call release.
     *
     * @param[in] ssic true for SSIC release.
     * @param[in] call The call window.
     */
    void callRelease(bool ssic, CallWindow *call = 0);

#ifdef DEBUG
    //for quick style change testing
    void testStyle(int style) { setStyle(style); }
#endif

    /**
     * Sets static members and creates the tab widget.
     *
     * @param[in] logger App logger.
     * @param[in] rscs   The Resources module.
     * @param[in] parent The parent widget, which is also the event filter
     *                   object for drag-and-drop.
     * @return The tab widget.
     */
    static QWidget *init(Logger *logger, Resources *rscs, QObject *parent);

    /**
     * Applies color theme to UI components.
     */
    static void setTheme();

    static void setMonMax(int maxGssi, int maxIssi)
    {
        sMonMaxMap[true] = maxGssi;
        sMonMaxMap[false] = maxIssi;
    }

    static void setUsername(const std::string &name) { sUsername = name; }

    /**
     * Sets the server session. Optionally saves data only if session is 0.
     *
     * @param[in] session  The server session.
     * @param[in] doSave   true to save data - only if session==0.
     * @param[in] onLogout true if called at logout without exit - applicable
     *                     only if session==0.
     */
    static void setSession(ServerSession *session,
                           bool           doSave = false,
                           bool           onLogout = false);

    static bool empty() { return sContactMap.empty(); }

    static bool hasId(int id) { return sContactMap.count(id) != 0; }

    /**
     * Adds contacts to a tab.
     *
     * @param[out] ctcs    The created contacts.
     * @param[in]  type    ID type - ResourceData::eType.
     * @param[in]  usr     true if added by user.
     * @param[in]  monMap  The IDs and their monitoring setting.
     * @param[in]  tabName The tab name, if any.
     * @return true if output is not empty.
     */
    static bool add(ContactsT     &ctcs,
                    int            type,
                    bool           usr,
                    const MonMapT &monMap,
                    const QString &tabName =  "");

    /**
     * If contacts exist, restores monitoring on them.
     * Otherwise restores contacts from saved data if configured to do so.
     *
     * @param[out] ctcs The created contacts.
     * @return true if output is not empty.
     */
    static bool dataRestore(ContactsT &ctcs);

    /**
     * Gets the contact with the given ID.
     *
     * @param[in] id The ID.
     * @return The contact, or 0 if not found.
     */
    static Contact *get(int id);

    /**
     * Gets all contacts associated with an ID.
     *
     * @param[out] ctcs The contacts.
     * @param[in]  id1  The ID.
     * @param[in]  id2  The second ID, if any.
     * @return true if output is not empty.
     */
    static bool getAll(ContactsT &ctcs, int id1, int id2 = 0);

    /**
     * Gets all contacts associated with a call.
     *
     * @param[out] ctcs The contacts.
     * @param[in]  call The call window.
     * @return true if output is not empty.
     */
    static bool getAll(ContactsT &ctcs, CallWindow *call);

    /**
     * Activates or deactivates a contact.
     *
     * @param[in] id     The ID.
     * @param[in] active true to activate.
     */
    static void activate(int id, bool active);

    /**
     * Gets a contact to refresh its display name.
     *
     * @param[in] id The ID.
     */
    static void refresh(int id);

    /**
     * Goes to the tab that contains a contact and makes it visible.
     *
     * @param[in] id The ID.
     */
    static void show(int id);

    /**
     * Moves a contact to a new position within the tab.
     *
     * @param[in] ctc    The contact to move.
     * @param[in] dstCtc The contact at the new position.
     */
    static void move(Contact *ctc, Contact *dstCtc);

    /**
     * Starts or stops monitoring contacts.
     *
     * @param[in] isGrp true for group IDs.
     * @param[in] start true to start.
     * @param[in] ids   The IDs.
     */
    static void monitor(bool isGrp, bool start, const ResourceData::IdsT &ids);

    /**
     * Restarts monitoring contacts with desired state not matching actual
     * state, i.e. those that failed in previous start request.
     */
    static void monitorRestore();

    /**
     * Checks whether an ID is monitored.
     *
     * @param[in]  id The resource ID.
     * @return true if monitored.
     */
    static bool monitored(int id);

    /**
     * Sets or clears the active group indicator on some contacts.
     *
     * @param[in] ids The IDs.
     */
    static void setGrpActive(const ResourceData::IdsT &ids);

    /**
     * Handles a call event.
     *
     * @param[in] type MsgSp::Type.
     * @param[in] call The call window.
     */
    static void callEvent(int type, CallWindow *call);

    /**
     * Calls rscDspOptChanged() on all contacts, and re-sorts sorted tabs.
     *
     * @param[in] type The affected resource type - ResourceData::eType.
     *                 TYPE_GROUP is also for DGNA groups.
     */
    static void onRscDspOptChanged(int type);

    /**
     * Handles drag and drop events.
     *
     * @param[in]  dst      The drop destination.
     * @param[in]  evt      The event.
     * @param[out] complete true if the event is completely handled.
     * @param[out] ctcs     The created contacts, if any.
     * @return true if the event is handled.
     */
    static bool doEventFilter(QWidget   *dst,
                              QEvent    *evt,
                              bool      &complete,
                              ContactsT &ctcs);

signals:
    void newCall(int calledType, int ssi, bool doPtt);
    void startAction(int actType, int idType, const ResourceData::IdsT &ids);

protected:
    //override
    void mousePressEvent(QMouseEvent *event);

    //override
    void mouseMoveEvent(QMouseEvent *event);

private:
    typedef std::set<int>                IntSetT;
    typedef std::map<int, QPushButton *> BtnMapT;      //key=eBtn
    typedef std::map<int, Contact *>     ContactMapT;  //key=ID
    typedef std::map<bool, int>          MonCountMapT; //true for groups
    //data for each tab
    struct TabData
    {
        TabData(bool isUsr) : usr(isUsr) {}
        TabData() {}

        bool             usr    = true;
        bool             sorted = false;
        QList<Contact *> ctcList; //use QList because it has removeOne()
        ContactsT        selection;
    };
    typedef std::map<QScrollArea *, TabData> TabMapT; //data for each tab

    Ui::Contact *ui;
    CallWindow  *mCall;
    int          mId;
    int          mStyle;        //eStyle
    bool         mActive;       //true if active
    bool         mIsGrp;
    bool         mMonitored;    //desired state, may be different from actual
    std::string  mLogPrefix;
    QString      mName;         //the display name
    QString      mNotes;
    QString      mSsIdlePttBtn; //idle PTT button stylesheet
    QTime        mCallTime;
    QTimer       mCallTimer;
    QTimer       mPttTimer;     //protect against PTT click as opposed to press
    //menu actions to show only in activated Contact
    std::set<QAction *> mActiveActions;

    static std::string    sUsername;    //for DB
    static Logger        *sLogger;
    static ServerSession *sSession;     //for monitoring
    static Resources     *sResources;
    static QObject       *sEventFilter; //event filter for drag-and-drop
    static QTabWidget    *sTabWidget;   //main UI container
    static ContactMapT    sContactMap;  //main contacts storage
    static TabMapT        sTabMap;
    static MonCountMapT   sMonMaxMap;   //monitoring limits
    static MonCountMapT   sMonCountMap; //monitoring count

    /**
     * Constructor is private to prevent external creation.
     *
     * @param[in] type Resource type - ResourceData::eType.
     * @param[in] id   Resource ID.
     * @param[in] usr  true if created by user.
     * @param[in] tab  Parent tab.
     */
    explicit Contact(int type, int id, bool usr, QScrollArea *tab);

    /**
     * Selects or deselects this contact, only if the selection checkbox is
     * present.
     *
     * @param[in] selected true to select.
     */
    void select(bool selected);

    /**
     * Activates or deactivates.
     *
     * @param[in] active   true to activate.
     * @param[in] onDelete true if this instance is about to be deleted.
     */
    void activate(bool active, bool onDelete = false);

    /**
     * Enables or disables UI components based on active and online status.
     */
    void setOnline();

    /**
     * Sets contact as system-defined, possibly changing from user-defined.
     */
    void setSysDefined();

    /**
     * Updates the title because of a change in the resource display text option.
     *
     * @param[in] type The affected resource type - ResourceData::eType.
     *                 TYPE_GROUP is also for DGNA groups.
     */
    void rscDspOptChanged(int type);

    /**
     * Sets the frame styles.
     *
     * @param[in] style The base style - eStyle. Omit when changing theme.
     */
    void setStyle(int style = -1);

    /**
     * Shows the notes dialog.
     */
    void showNotes();

    /**
     * Gets a tab, creating one if necessary.
     * If the tab text is given:
     *  -Returns the tab if found, otherwise creates one.
     * Otherwise:
     *  -If current tab matches the type (user/system), returns it.
     *  -Else if the type tab exists, returns the first one.
     *  -Else for user type, creates one.
     *  -Else for system type, returns 0.
     *
     * @param[in] usr true for user tab, otherwise system tab.
     * @param[in] txt The tab text.
     * @return The tab.
     */
    static QScrollArea *tabGet(bool usr, const QString &txt = "");

    /**
     * Shows a dialog for either tab creation or renaming.
     *
     * @param[in] idx The tab index. Omit for creation.
     */
    static void tabName(int idx = -1);

    /**
     * Validates a new tab name by checking that it does not:
     *  -contain disallowed characters,
     *  -match an existing name, with case-insensitive comparison.
     *
     * @param[in] name The name.
     * @return true if valid.
     */
    static bool tabValidate(const QString &name);

    /**
     * Adds an item, creating one if necessary, to a tab.
     *
     * @param[in] tabTxt  The tab text.
     * @param[in] type    The resource type - ResourceData::eType.
     * @param[in] id      The ID.
     * @param[in] usr     true if created by user.
     * @param[in] offline true if resource is offline.
     * @param[in] doSave  true to save data.
     * @return true if a Contact was created.
     */
    static bool tabAddItem(const QString &tabTxt,
                           int            type,
                           int            id,
                           bool           usr,
                           bool           offline,
                           bool           doSave);

    /**
     * Adds an item to a tab.
     *
     * @param[in] tab    The tab.
     * @param[in] ctc    The item.
     * @param[in] doSave true to save data.
     */
    static void tabAddItem(QScrollArea *tab, Contact *ctc, bool doSave);

    /**
     * Moves a tab's selected items to another tab.
     *
     * @param[in] tab The destination tab.
     * @param[in] src The source tab.
     */
    static void tabMoveItems(QScrollArea *tab, QScrollArea *src);

    /**
     * Lays out items in one or all tabs.
     * 'All tabs' option is for re-sorting by onRscDspOptChanged() - skipping
     * the unsorted ones.
     *
     * @param[in] tab The tab, or 0 for all tabs.
     */
    static void tabLayout(QScrollArea *tab = 0);

    /**
     * Deletes selected items from a tab, and optionally deletes the tab.
     *
     * @param[in] tab       The tab.
     * @param[in] deleteTab true to delete the tab.
     */
    static void tabDelete(QScrollArea *tab, bool deleteTab);

    /**
     * Adds menu item to move either this object or selected objects in the
     * current tab to another tab. Does nothing if no target tab available.
     *
     * @param[in,out] menu The menu.
     * @param[in]     ctc  Calling Contact instance, if any.
     * @return true if menu item added.
     */
    static bool menuMoveToTab(QMenu &menu, Contact *ctc = 0);

    /**
     * Checks whether a drag and drop event is acceptable.
     *
     * @param[in] src The source object being dragged.
     * @param[in] dst The destination widget.
     * @param[in] pos The drop position.
     * @return true if acceptable.
     */
    static bool canDrop(QObject *src, QWidget *dst, const QPoint &pos);

    /**
     * Saves data to database - adds/deletes/updates either a particular contact
     * or tab, or all data if neither specified. Tab update is for renaming.
     *
     * @param[in] mode Positive to add, negative to delete, 0 to update.
     * @param[in] ctc  The contact.
     * @param[in] tab  The tab - used only if ctc is 0.
     * @param[in] old  The tab's old name for a tab update.
     */
    static void dataSave(int          mode,
                         Contact     *ctc,
                         QScrollArea *tab = 0,
                         QString     *old = 0);
};
#endif //CONTACT_H
