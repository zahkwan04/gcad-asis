/**
 * Function implementations related to location and map boundaries.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLocation.cpp 1899 2025-01-22 05:42:07Z rosnin $
 * @author Mohd Rozaimi
 * @author Zunnur Zafirah
 */
#include "Utils.h"
#include "GisLocation.h"

using namespace std;

static const string BOUNDS("[" + Utils::toString(GisLocation::BOUND_BTM) + "," +
                           Utils::toString(GisLocation::BOUND_TOP) + "],[" +
                           Utils::toString(GisLocation::BOUND_LEFT) + "," +
                           Utils::toString(GisLocation::BOUND_RIGHT) + "]");

const double GisLocation::BOUND_TOP   = 8.400;
const double GisLocation::BOUND_BTM   = 0.0;
const double GisLocation::BOUND_LEFT  = 97.193;
const double GisLocation::BOUND_RIGHT = 120.00;
const double GisLocation::MAP_SCALE   = 7300000;

std::string GisLocation::getBounds()
{
    return BOUNDS;
}

bool GisLocation::isInBounds(double lat, double lon)
{
    return (lat >= BOUND_BTM && lat <= BOUND_TOP &&
            lon >= BOUND_LEFT && lon <= BOUND_RIGHT);
}

string GisLocation::getCoord(double val, int pre)
{
    return Utils::toString(val, pre);
}

int GisLocation::getCoordMinSec(double &val)
{
    if (val < 0)
        val = -val;
    int n = val; //degrees
    val = (val - n) * 60.0; //decimal minutes
    n = val; //minutes
    val = (val - n) * 60.0; //decimal seconds
    return n;
}
