/**
 * Class for terminal tracking replay.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTrackingReplay.h 1819 2024-02-29 01:50:24Z rosnin $
 * @author Zunnur Zafirah
 */
#ifndef GISTRACKINGREPLAY_H
#define GISTRACKINGREPLAY_H

#include <QDialog>
#include <QSet>
#include <QTimer>

#include "DbInt.h"
#include "GisCanvas.h"

namespace Ui {
class GisTrackingReplay;
}

class GisTrackingReplay : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] canvas The map canvas.
     * @param[in] parent Parent widget, if any.
     */
    GisTrackingReplay(GisCanvas *canvas, QWidget *parent = 0);

    ~GisTrackingReplay();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Resets the replay data.
     */
    void resetData();

    /**
     * Starts the replay.
     */
    void start();

    /**
     * Adds replay data.
     * resetData() must have been called before the first call to this.
     *
     * @param[in] issi The ISSI.
     * @param[in] res  The tracking data.
     */
    void addData(int issi, DbInt::QResult *res);

private:
    //key is ISSI, value is coordinates
    typedef QMap<int, std::string>  IssiPointsT;
    typedef QMap<uint, IssiPointsT> TrackPointsT; //key is time_t

    Ui::GisTrackingReplay  *ui;
    QTimer                 *mTimer;
    GisCanvas              *mCanvas;
    int                     mSpeed;
    int                     mPointNum;
    uint                    mTimeStart;
    uint                    mTimeEnd;
    QSet<int>               mIssis;
    TrackPointsT            mTrackpoints;
    TrackPointsT::iterator  mIt;

    /**
     * Resets the replay to the beginning, and adds the first point.
     * If this is the first reset for the current data, enables auto-zoom when
     * adding the first point.
     *
     * @param[in] isFirstTime true for the first reset for the current data.
     */
    void reset(bool isFirstTime);

    /**
     * Stops replay and deletes all tracking lines.
     *
     * @param[in] doReset true to invoke reset.
     */
    void stop(bool doReset);

    /**
     * Updates the position label based on the last plotted point.
     */
    void updatePositionLabel();
};
#endif //GISTRACKINGREPLAY_H
