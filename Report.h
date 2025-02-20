/**
 * The Report UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Report.h 1799 2023-12-22 09:21:23Z hazim.rujhan $
 * @author Zulzaidi Atan
 */
#ifndef REPORT_H
#define REPORT_H

#include <set>
#include <QCompleter>
#include <QWidget>

#include "AudioPlayer.h"
#include "DateTimeDelegate.h"
#ifdef INCIDENT
#include "IncidentData.h"
#endif
#include "Logger.h"

namespace Ui {
class Report;
}

class Report : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] audioPlayer The shared audio player. Takes ownership.
     * @param[in] logger      A logger object. Caller retains ownership.
     * @param[in] parent      Parent widget, if any.
     */
    explicit Report(AudioPlayer *audioPlayer,
                    Logger      *logger,
                    QWidget     *parent = 0);

    ~Report();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Performs cleanup on logout.
     */
    void handleLogout();

    /**
     * Retrieves data from database.
     */
    void retrieveData();

#ifdef INCIDENT
signals:
    void clearIncidents();
    void plotIncidents(const std::set<IncidentData *> &data,
                       const std::set<int>            &ids);
    void showIncidentById(int id);
    void showIncidentByData(IncidentData *data);
#endif

private:
    enum eFilterType
    {
        FILTERTYPE_CALL,
        FILTERTYPE_INCIDENT,
        FILTERTYPE_LOC,
        FILTERTYPE_MMS,
        FILTERTYPE_MSG,
        FILTERTYPE_SDS,
        FILTERTYPE_STS
    };

    Ui::Report       *ui;
    AudioPlayer      *mAudioPlayer;
    QCompleter       *mCompleter;
    Logger           *mLogger;
    DateTimeDelegate *mDateTimeDelegate;
    int               mFilterType;

#ifdef INCIDENT
    /**
     * Plots/clears incidents on map.
     *
     * @param[in] plot true to plot.
     */
    void onPlotCheck(bool plot);
#endif

    /**
     * Displays report.
     */
    void onDisplay();

    /**
     * Shows fields for a filter type and hides others.
     *
     * @param[in] type eFilterType.
     */
    void showFilter(int type);

    /**
     * Validates fields with the following rules:
     *  -End date and time must not be earlier than the start.
     *  -Start and end date difference must not exceed the MAX_DAYS set.
     *  -From can be empty or contain a positive integer.
     *  -To can be empty or contain a positive integer or a valid group name.
     *
     * @param[out] from The validated From ID.
     * @param[out] to   The validated To ID or GSSI of a group name.
     * @return true if valid.
     */
    bool validateFields(int &from, int &to);

    /**
     * Sets the result header title and tool tip of the current displayed
     * report.
     */
    void setTitleAndToolTip();

    /**
     * Gets a printable document for the current displayed report and shows
     * print preview or export dialog.
     *
     * @param[in] printType The print document type. See Document::ePrintType.
     */
    void doPrint(int printType);

    /**
     * Performs table data cleanup.
     */
    void cleanup();
};
#endif //REPORT_H
