/**
 * Map view history manager.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisHistory.h 1898 2025-01-17 00:50:10Z rosnin $
 * @author Zunnur Zafirah
 */
#ifndef GISHISTORY_H
#define GISHISTORY_H

#include <QObject>
#include <QPointF>
#include <list>

#include "GisCanvas.h"

class GisHistory : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] canvas The canvas.
     */
    GisHistory(GisCanvas *canvas);

    /**
     * Saves current map view to history. First removes all views beyond the
     * current one, if any. Or removes the oldest view if the history list
     * has reached the maximum size.
     *
     * @param[in] center  The center coordinates.
     * @param[in] zoomLvl The zoom level.
     * @return true if the saved view is not the first.
     */
    bool save(QPointF center, double zoomLvl);

    /**
     * Navigates to previous map view in history.
     *
     * @return true if not at the beginning.
     */
    bool prev();

    /**
     * Navigates to next map view in history.
     *
     * @return true if not at the end.
     */
    bool next();

    /**
     * Gets the current navigation flag, which indicates whether the last
     * view change is caused by history navigation. Toggles it if true (but
     * returns the original value).
     *
     * @return The navigation flag.
     */
    bool checkNavigation();

private:
    struct ViewData
    {
        ViewData(QPointF ctr, int zoom) : center(ctr), zoomLvl(zoom) {}

        QPointF center;
        double  zoomLvl;
    };
    typedef std::list<ViewData> ViewListT;

    bool                 mNavigate; //indicates ongoing history navigation
    ViewListT            mViewList;
    ViewListT::iterator  mViewListIt;
    GisCanvas           *mMapCanvas;
};
#endif //GISHISTORY_H
