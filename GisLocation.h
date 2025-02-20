/**
 * Platform-independent constants and functions related to location and map
 * boundaries.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLocation.h 1899 2025-01-22 05:42:07Z rosnin $
 * @author Mohd Rozaimi
 * @author Zunnur Zafirah
 */
#ifndef GISLOCATION_H
#define GISLOCATION_H

#include <string>

class GisLocation
{
public:
    enum eCoord
    {
        COORD_NONE,    //not coordinates
        COORD_INVALID, //coordinates beyond ([-90,90], [-180,180])
        COORD_VALID
    };

    static const int PRECISION = 6; //for coordinate string

    //map boundaries
    static const double BOUND_TOP;
    static const double BOUND_BTM;
    static const double BOUND_LEFT;
    static const double BOUND_RIGHT;
    static const double MAP_SCALE; //initial scale

    /**
     * Gets the map boundaries.
     *
     * @return The boundaries as "[<bottom>,<top>],[<left>,<right>]".
     */
    static std::string getBounds();

    /**
     * Checks whether coordinates are within the map boundaries.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @return true if within.
     */
    static bool isInBounds(double lat, double lon);

    /**
     * Converts a single coordinate value to string.
     *
     * @param[in] val The value.
     * @param[in] pre The floating point precision.
     * @return The string.
     */
    static std::string getCoord(double val, int pre = PRECISION);

    /**
     * Gets the minutes and seconds of a decimal degree value. Outputs are
     * always positive.
     *
     * @param[in,out] val In: Degrees. Out: Seconds
     * @return Minutes.
     */
    static int getCoordMinSec(double &val);
};
#endif //GISLOCATION_H
