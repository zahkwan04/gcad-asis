/**
 * Qt-specific location point-related functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: GisPoint.h 1792 2023-12-07 07:17:36Z rosnin $
 * @author Mohd Rozaimi
 * @author Zunnur Zafirah
 */
#ifndef GISPOINT_H
#define GISPOINT_H

#include <QPointF>
#include <QString>
#include <QWidget>

#include "GisLocation.h"

class GisPoint : public GisLocation
{
public:
    /**
     * Checks whether a string represents coordinates, and returns the
     * coordinates if so.
     * Supported formats:
     *   -Decimal degrees:         [-]DD.ddd(,| )[-]DDD.ddd
     *                             2.9383,-101.5729
     *   -Degrees decimal-minutes: DD MM.mmm (N|S)(,| )DDD MM.mmm (E|W)
     *                             2 56.298 N 101 34.374 W
     *   -Degrees minutes seconds:
     *                 DD [MM [ss[.sss]]] (N|S)(,| )DDD [MM [ss[.sss]]] (E|W)
     *                 2 56 17 N 101 34 22.5 W
     * Degree/minute/second symbols may be present immediately after the
     * respective values.
     *
     * @param[in]  str   The string. Must not contain leading or trailing
     *                   spaces.
     * @param[out] point The point coordinates. Unmodified if none found.
     * @return GisLocation::eCoord value.
     */
    static int checkCoords(const QString &str, QPointF &point);

    /**
     * Checks whether a string represents valid coordinates within the map
     * boundaries, using the other checkCoords().
     * Shows an error dialog for values beyond the permitted range
     * (see COORD_INVALID), and optionally for COORD_NONE.
     *
     * @param[in]  parent      The parent widget for any error dialog.
     * @param[in]  errTitle    The error dialog title.
     * @param[in]  str         The string. Must not contain leading or trailing
     *                         spaces.
     * @param[in]  isMandatory true to show an error dialog for COORD_NONE.
     * @param[out] point       The valid coordinates.
     *                         Unmodified if none found.
     * @return GisLocation::eCoord value. Also COORD_INVALID for valid
     *         coordinates beyond map boundaries.
     */
    static int checkCoords(QWidget       *parent,
                           const QString &errTitle,
                           const QString &str,
                           bool           isMandatory,
                           QPointF       &point);

    /**
     * Gets a point's latitude as string using GisLocation::getCoord().
     *
     * @param[in] point The point.
     * @return The string.
     */
    static std::string getLat(const QPointF &point);

    /**
     * Gets a point's longitude as string using GisLocation::getCoord().
     *
     * @param[in] point The point.
     * @return The string.
     */
    static std::string getLon(const QPointF &point);

    /**
     * Converts a pair of coordinates to string using GisLocation::getCoord().
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @param[in] pre The floating point precision.
     * @return String in the form "lat,lon".
     */
    static QString getCoords(double lat,
                             double lon,
                             int    pre = GisLocation::PRECISION);

    /**
     * Gets a point's coordinates as string.
     *
     * @param[in] point The point.
     * @param[in] dec   true for decimal format.
     * @return String from getCoords(double, ...) for decimal format, or
     *         getCoordsDms() otherwise.
     */
    static QString getCoords(const QPointF &point, bool dec = true);

    /**
     * Converts a pair of coordinates to string as:
     *   deg min' sec.ss" N/S, deg min' sec.ss" E/W
     *   (deg is followed by the degree symbol)
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @return The formatted string.
     */
    static QString getCoordsDms(double lat, double lon);

    /**
     * Gets the state and address at a point.
     *
     * @param[in]  lat   The latitude.
     * @param[in]  lon   The longitude.
     * @param[out] addr  The address, if any.
     * @param[out] state The state, if any.
     * @return true if successful to get state.
     */
    static bool getAddress(double       lat,
                           double       lon,
                           std::string &addr,
                           std::string &state);

    /**
     * Gets line rotation degree.
     *
     * @param[in] startLat The start point latitude.
     * @param[in] startLon The start point longitude.
     * @param[in] endLat   The end point latitude.
     * @param[in] endLon   The end point longitude.
     * @return The degree of line rotation.
     */
    static double getLineDegree(double startLat,
                                double startLon,
                                double endLat,
                                double endLon);
};
#endif //GISPOINT_H
