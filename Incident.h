/**
 * The Incident UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Incident.h 1862 2024-06-04 06:33:19Z rosnin $
 * @author Mazdiana Makmor
 */
#ifndef INCIDENT_H
#define INCIDENT_H

#include <map>
#include <set>
#include <QModelIndex>
#include <QPointF>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringList>
#include <QStringListModel>
#include <QTableView>
#include <QWidget>

#include "DbInt.h"
#include "ActiveIncidents.h"
#include "AudioPlayer.h"
#include "DraggableListView.h"
#include "GisWindow.h"
#include "IncidentButton.h"
#include "IncidentData.h"
#include "Logger.h"
#include "ResourceData.h"
#include "ServerSession.h"

namespace Ui {
class Incident;
}

class Incident : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] audioPlayer The shared audio player.
     * @param[in] logger      App logger.
     * @param[in] activeInc   Takes ownership. If 0, created here.
     * @param[in] parent      Parent widget, if any.
     */
    explicit Incident(AudioPlayer     *audioPlayer,
                      Logger          *logger,
                      ActiveIncidents *activeInc,
                      QWidget         *parent = 0);

    ~Incident();

    /**
     * Applies color theme to UI components.
     *
     * @param[in] init true if called from init().
     */
    void setTheme(bool init = false);

    /**
     * Sets the session, and creates resource list models if not yet created, or
     * if resource type is changed based on current operating mode.
     *
     * @param[in] session Server session.
     */
    void init(ServerSession *session);

    /**
     * Sets or clears the GisWindow component.
     *
     * @param[in] gis The GisWindow, or 0 to clear.
     */
    void setGis(GisWindow *gis);

    void setServerIssi(int issi) { mServerIssi = issi; }

    /**
     * Resets all fields and buttons.
     */
    void reset();

    void setUserName(const QString &userName);

    /**
     * Enables or disables the buttons for resource selection on map.
     *
     * @param[in] enabled true to enable.
     */
    void enableResourceSelect(bool enabled = true);

    /**
     * Gets the ID of incident being edited.
     *
     * @return The ID or 0 if no incident being edited.
     */
    int getEditId();

    /**
     * Retrieves data from database.
     *
     * @param[in] username The username.
     */
    void retrieveData(const QString &username);

    /**
     * Loads incident data from database.
     *
     * @param[in] id The incident ID. Omit to load all open incidents.
     */
    void loadData(int id = 0);

    /**
     * Shows incident data in the UI.
     *
     * @param[in] btn The clicked button.
     */
    void showData(IncidentButton *btn);

    /**
     * Shows incident data in the UI.
     *
     * @param[in] data The data.
     * @return true if a dialog box was shown.
     */
    bool showData(IncidentData *data);

    /**
     * Shows incident data in the UI. Shows an error dialog for invalid ID.
     *
     * @param[in] id The incident ID.
     * @return true for valid ID.
     */
    bool showData(int id);

    /**
     * Sets or clears an incident lock holder.
     * If the incident is displayed now, enables editing if lock is cleared, and
     * disables it otherwise.
     *
     * @param[in] id         The incident ID.
     * @param[in] lockHolder The lock holder ID. 0 to clear.
     * @param[in] unlocked   true if incident is now unlocked.
     */
    void setLockHolder(int id, int lockHolder, bool unlocked);

    /**
     * Locks or unlocks the UI for editing.
     *
     * @param[in] id     The incident ID.
     * @param[in] doLock true to lock.
     */
    void editLock(int id, bool doLock);

    /**
     * Reverts editing lock state.
     *
     * @param[in] enableEdit true to enable edit.
     */
    void editLockRevert(bool enableEdit);

    /**
     * Performs cleanup on logout.
     */
    void handleLogout();

signals:
    void assignedResources(int idType, const ResourceData::IdsT &ids, int id);
    void sendSds(int                       idType,
                 const ResourceData::IdsT &ids,
                 const QStringList        &msgs,
                 const QString            &txt);
    void startAction(int actType, int idType, const ResourceData::IdsT &r);

public slots:
    /**
     * Assigns the selected resource ISSIs to the incident.
     *
     * @param[in] mdl The resource model. Takes ownership.
     */
    void onResourceAssign(ResourceData::ListModel *mdl);

    /**
     * Shows incident coordinates.
     *
     * @param[in] x X coordinate.
     * @param[in] y Y coordinate.
     */
    void showIncidentCoord(double x, double y);

    /**
     * Handles new call, SDS or Status Message event.
     *
     * @param[in] type The communication type - CmnTypes::eCommType.
     *                 COMM_MSG_SDS/STATUS for SDS/Status, and any other value
     *                 for a call.
     * @param[in] from Calling/sending party.
     * @param[in] to   Called/receiving party.
     */
    void onNewComm(int type, const QString &from, const QString &to);

    /**
     * Plots incidents on map.
     *
     * @param[in] data Incident data.
     * @param[in] ids  Incident IDs.
     */
    void onPlotData(const std::set<IncidentData *> &data,
                    const std::set<int>            &ids);

private slots:
    /**
     * Marks whether the source text box content has changed against the saved
     * settings. Disregards any leading and trailing spaces.
     *
     * @param[in] text The text.
     */
    void onTextChanged(const QString &text);

    /**
     * Marks whether the source date box content has changed against the saved
     * settings.
     *
     * @param[in] date The date.
     */
    void onDateChanged(const QDate &date);

    /**
     * Marks whether the source time box content has changed against the saved
     * settings.
     *
     * @param[in] time The time.
     */
    void onTimeChanged(const QTime &time);

protected:
    //override
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Incident                   *ui;
    ActiveIncidents                *mActiveInc;
    AudioPlayer                    *mAudioPlayer;
    Logger                         *mLogger;
    ServerSession                  *mSession;
    GisWindow                      *mGis;
    QSortFilterProxyModel          *mTrmMdl;  //only in full mode
    QSortFilterProxyModel          *mMobMdl;
    DraggableListView              *mTrmList; //only in full mode
    DraggableListView              *mMobList;
    DraggableListView              *mRscAssigned;
    IncidentData                   *mData;
    int                             mServerIssi;
    bool                            mRscSelEnabled;
    QString                         mUserName;
    std::set<int>                   mUpdatedFields;
    //key: state name, value: state code
    std::map<QString, std::string>  mStates;

    /**
     * Clears some fields to allow creation of a new incident.
     *
     * @return true if operation is not cancelled by user.
     */
    bool startNew();

    /**
     * Initializes all date and time fields to the current time.
     * Also sets the minimum and maximum dates.
     */
    void setCurrentDateTime();

    /**
     * Tries to look up the state and address at a point and if successful, sets
     * them in the respective UI fields.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void lookupAddress(double lat, double lon);

    /**
     * Sets data from UI.
     */
    void setData();

    /**
     * Notifies relevant modules on Incident update.
     *
     * @param[in] data The Incident data.
     */
    void updateCase(IncidentData *data);

    /**
     * Unlocks editing if necessary, and notifies relevant modules on Incident
     * closure.
     *
     * @param[in] id The Incident ID.
     */
    void closeCase(int id);

    /**
     * Enables or disables status date and time fields based on the selected
     * status. Also enables or disables the Closed radio button.
     *
     * @param[in] enableDisp    true to enable Dispatched fields.
     * @param[in] enableOnScene true to enable On Scene fields.
     * @param[in] enableClosed  true to enable Closed fields.
     * @param[in] status        The status.
     */
    void setStatusFields(bool enableDisp,
                         bool enableOnScene,
                         bool enableClosed,
                         int  status);

    /**
     * Adds a field to the list if the field value has been changed.
     * Otherwise removes the field.
     *
     * @param[in] field   The field.
     * @param[in] changed true if the value has changed.
     */
    void addRemoveField(int field, bool changed);

    /**
     * Updates status, dates and times in the given record.
     *
     * @param[in,out] data The record.
     * @return The status name.
     */
    std::string updateStatusDateTime(DbInt::RecordT &data);

    /**
     * Gets resources from the UI.
     *
     * @param[out] resources The resource ISSI list.
     */
    void getResources(std::set<std::string> &resources);

    /**
     * Performs the necessary actions after new resource assignments.
     *
     * @param[in] ids The newly assigned resource IDs.
     */
    void handleResourceAssign(const ResourceData::IdsT &ids);

    /**
     * Validates the user data.
     *
     * @return true if valid.
     */
    bool validate();

    /**
     * Validates the latitude and longitude user input.
     * Shows an error dialog if invalid.
     *
     * @param[out] pt The validated point, if needed.
     * @return true if valid.
     */
    bool validateCoords(QPointF *pt = 0);

    /**
     * Validates the status dates and times with the following rules:
     *  -Each status date must not be earlier than the previous ones.
     *  -Each status time must be later than the previous ones.
     *
     * @param[in] status The status.
     * @return true if valid.
     */
    bool validateStatusDateTime(int status);

    /**
     * Sets the UI states of status radio buttons, dates and times.
     *
     * @param[in] data      The data.
     * @param[in] doSetData true to set the status dates and times.
     */
    void setStatusUiStates(IncidentData *data, bool doSetData);

    /**
     * Sets editing mode.
     *
     * @param[in] editable true to enable editing.
     */
    void setEditable(bool editable);

    /**
     * Gets a printable document for the current displayed incident and shows
     * print preview or export dialog.
     *
     * @param[in] printType The print document type - Document::ePrintType.
     */
    void doPrint(int printType);

    /**
     * Formats a duration value in days, hours and minutes.
     * E.g. "2 days 1 hour 12 minutes".
     * A convenience function for doPrint().
     *
     * @param[in] t The duration in seconds.
     * @return Empty string if t is less than 1 minute. The formatted string
     *         otherwise.
     */
    QString formatDuration(int t);

    /**
     * Enables or disables the "Send Now..." SDS button menu items based on
     * whether the incident has been created and resources assigned.
     */
    void enableDisableSendNow();

    /**
     * Creates SDS/MMS messages based on auto-generated text options to send to
     * assigned resources.
     *
     * @param[out] txt The complete message text for MMS - assigned only if
     *                 ResourceData::hasMobileStat().
     * @return The messages. Empty if incident not yet created (no ID).
     */
    QStringList getSdsMsg(QString &txt);

    /**
     * Triggers SDS notification to assigned resources, using either
     * auto-generated or user-edited text.
     *
     * @param[in] autoText true for auto-generated text.
     */
    void notifyResources(bool autoText);

    /**
     * Checks whether a communication event is relevant to the current incident
     * based on the sender, recipient and assigned resources.
     * If so, gets the last data from database into a table. And if this adds
     * the first row into the table, makes its container frame visible.
     *
     * @param[in]  commType The communication type - CmnTypes::eCommsType.
     * @param[in]  from     The sender.
     * @param[in]  to       The recipient.
     * @param[in]  frame    The frame containing the table.
     * @param[out] tv       The table.
     */
    void checkRelevantComm(int            commType,
                           const QString &from,
                           const QString &to,
                           QFrame        *frame,
                           QTableView    *tv);
};
#endif //INCIDENT_H
