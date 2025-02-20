/**
 * GPS monitoring selector module.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: GpsMonitor.h 1771 2023-10-19 07:48:56Z hazim.rujhan $
 * @author Mohd Fashan Abdul Munir
 * @author Mohd Rozaimi
 */
#ifndef GPSMONITOR_H
#define GPSMONITOR_H

#include <string>
#include <QDialog>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "DraggableListView.h"
#include "Logger.h"
#include "ServerSession.h"

namespace Ui {
class GpsMonitor;
}

class GpsMonitor : public QDialog
{
    Q_OBJECT

public:
    typedef ServerSession::SsiSetT SsisT;

    /**
     * Constructor.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit GpsMonitor(QWidget *parent = 0);

    ~GpsMonitor();

    /**
     * Initializes the module.
     *
     * @param[in] logger   The logger.
     * @param[in] session  The server session for sending messages.
     * @param[in] issiList The current configured monitoring list.
     */
    static void init(Logger            *logger,
                     ServerSession     *session,
                     const std::string &issiList);

    static bool isMonGrps() { return sMonGrps; }

    /**
     * Handles a monitoring request result.
     *
     * @param[in,out] msg The GPS_MON_START/STOP result message.
     *                    On return, MSG_ACK field removed from GPS_MON_START
     *                    to indicate GPS monitoring change from all to
     *                    selection, and from GPS_MON_STOP to indicate change to
     *                    monitored groups.
     * @return true if successful.
     */
    static bool result(MsgSp &msg);

    /**
     * Starts monitoring attached group members of newly monitored groups - only
     * for members who are not already monitored. Does nothing if not in
     * monitored groups mode.
     *
     * @param[in] gssis Monitored groups.
     * @return Error message, empty if successful.
     */
    static QString monGrpsStart(const SsisT &gssis);

    /**
     * Stops monitoring attached group members of groups no longer monitored -
     * only for members who are not attached to other monitored groups. Does
     * nothing if not in monitored groups mode.
     *
     * @param[in]     gssis   Monitored groups.
     * @param[in,out] affSsis Affected SSIs.
     *                        In: GSSIs no longer monitored.
     *                        Out: ISSIs no longer monitored as a result.
     * @return Error message, empty if successful.
     */
    static QString monGrpsStop(const SsisT &gssis, SsisT &affSsis);

    /**
     * Starts/stops monitoring a group member that just got attached/detached.
     * Does nothing if not in monitored groups mode.
     *
     * @param[in] issi  The group member.
     * @param[in] gssis Monitored groups.
     * @return Error message, empty if successful.
     */
    static QString monGrpsAttDet(int issi, const SsisT &gssis);

    /**
     * Gets the monitoring list of comma-separated ISSI ranges.
     *
     * @param[in] forLog true for log, otherwise for settings. The difference is
     *                   only when monitoring group members - actual ISSIs not
     *                   saved in settings but replaced with special indicator.
     * @return The list.
     */
    static std::string getList(bool forLog = false);

signals:
    void listChanged(const std::string &ssis); //for change without svr msg
    void locateResource(int issi);

protected:
    //override
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::GpsMonitor        *ui;
    DraggableListView     *mResourceList;
    DraggableListView     *mMonList;
    QSortFilterProxyModel *mSortModel;
    int                    mOpt;

    static bool           sMonGrps; //true if monitoring grp members
    static SsisT          sIssis;   //the monitoring list
    static ServerSession *sSession;
    static Logger        *sLogger;

    /**
     * Enables or disables UI components for selecting ISSIs, based on the
     * selected monitoring option.
     *
     * @param[in] enabled true to enable.
     */
    void setEnabled(bool enabled);

    /**
     * Moves selected items between mResourceList and mMonList, and sorts the
     * destination list.
     *
     * @param[in] add true to move from mResourceList to mMonList, false for the
     *                other direction.
     */
    void moveItems(bool add);

    /**
     * OK button handler. Sends update request and closes the dialog.
     */
    void onOk();
};
#endif //GPSMONITOR_H
