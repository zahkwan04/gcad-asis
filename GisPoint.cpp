/**
 * Location point-related function implementations.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: GisPoint.cpp 1794 2023-12-18 09:06:22Z rosnin $
 * @author Mohd Rozaimi
 * @author Zunnur Zafirah
 */
#include <QMessageBox>
#include <QRegularExpression>

#include "DbInt.h"
#include "GisPoint.h"

using namespace std;

static const double PI = 3.14159265;

int GisPoint::checkCoords(const QString &str, QPointF &point)
{
    //decimal degrees:     [-]DD.ddd                [-]DDD.ddd
    //degree/minute/second symbols are optional in the formats below:
    //deg decimal-minutes: DD MM.mmm (N|S)          DDD MM.mmm (E|W)
    //deg minutes seconds: DD [MM [ss[.sss]]] (N|S) DDD [MM [ss[.sss]]] (E|W)
    static const QRegularExpression RE(
             "^((?<LatDegDec>-?\\d{1,2}\\.\\d+)|"
               "((?<LatDeg>\\d{1,2})\\D?\\s*"
                "((?<LatMinDec>\\d{1,2}\\.\\d+)'?|"
                 "(?<LatMin>\\d{0,2})'?\\s*(?<LatSec>\\d{0,2}(\\.\\d+)?)\"?)"
                ")\\s*(?<NS>[NS]))"
             "\\s*[\\s,]\\s*"
             "((?<LonDegDec>-?\\d{1,3}\\.\\d+)|"
              "((?<LonDeg>\\d{1,3})\\D?\\s*"
               "((?<LonMinDec>\\d{1,2}\\.\\d+)'?|"
                "(?<LonMin>\\d{0,2})'?\\s*(?<LonSec>\\d{0,2}(\\.\\d+)?)\"?)"
               ")\\s*(?<EW>[EW]))$");
    QRegularExpressionMatch match = RE.match(str);
    if (!match.hasMatch())
        return COORD_NONE;
    double y = -999.0;
    QString dec(match.captured("LatDegDec"));
    if (!dec.isEmpty())
    {
        y = dec.toDouble();
    }
    else
    {
        y = match.captured("LatDeg").toDouble();
        dec = match.captured("LatMinDec");
        if (!dec.isEmpty())
        {
            y += dec.toDouble()/60;
        }
        else
        {
            dec = match.captured("LatMin");
            if (!dec.isEmpty())
            {
                y += dec.toDouble()/60;
                dec = match.captured("LatSec");
                if (!dec.isEmpty())
                    y += dec.toDouble()/3600;
            }
        }
        if (match.captured("NS") == "S")
            y = -y;
    }
    if (y < -90 || y > 90)
        return COORD_INVALID;
    double x = -999.0;
    dec = match.captured("LonDegDec");
    if (!dec.isEmpty())
    {
        x = dec.toDouble();
    }
    else
    {
        x = match.captured("LonDeg").toDouble();
        dec = match.captured("LonMinDec");
        if (!dec.isEmpty())
        {
            x += dec.toDouble()/60;
        }
        else
        {
            dec = match.captured("LonMin");
            if (!dec.isEmpty())
            {
                x += dec.toDouble()/60;
                dec = match.captured("LonSec");
                if (!dec.isEmpty())
                    x += dec.toDouble()/3600;
            }
        }
        if (match.captured("EW") == "W")
            x = -x;
    }
    if (x < -180 || x > 180)
        return COORD_INVALID;
    point.setX(x);
    point.setY(y);
    return COORD_VALID;
}

int GisPoint::checkCoords(QWidget       *parent,
                          const QString &errTitle,
                          const QString &str,
                          bool           isMandatory,
                          QPointF       &point)
{
    int res = checkCoords(str, point);
    switch (res)
    {
        case COORD_INVALID:
            QMessageBox::critical(parent, errTitle,
                                  QObject::tr("Coordinates (%1) are invalid.")
                                      .arg(str),
                                  QMessageBox::Ok);
            break;
        case COORD_NONE:
            if (isMandatory)
                QMessageBox::critical(parent, errTitle,
                                      QObject::tr("Input does not represent "
                                                  "coordinates: %1").arg(str),
                                      QMessageBox::Ok);
            break;
        default:
            if (!isInBounds(point.y(), point.x()))
            {
                res = COORD_INVALID;
                QMessageBox::critical(parent, errTitle,
                                      QObject::tr("Coordinates (%1) are beyond "
                                                  "map boundaries (%2).")
                                          .arg(str,
                                               QString::fromStdString(
                                                                  getBounds())),
                                      QMessageBox::Ok);
            }
            break;
    }
    return res;
}

string GisPoint::getLat(const QPointF &point)
{
    return getCoord(point.y());
}

string GisPoint::getLon(const QPointF &point)
{
    return getCoord(point.x());
}

QString GisPoint::getCoords(double lat, double lon, int pre)
{
    return QString::fromStdString(getCoord(lat, pre) + "," + getCoord(lon, pre));
}

QString GisPoint::getCoords(const QPointF &point, bool dec)
{
    return (dec)? getCoords(point.y(), point.x()):
                  getCoordsDms(point.y(), point.x());
}

QString GisPoint::getCoordsDms(double lat, double lon)
{
    QString c((lat >= 0)? QObject::tr("N"): QObject::tr("S"));
    if (lat < 0)
        lat = -lat;
    QString s(QString::number((int) lat));
    //caution: cannot chain all append() below because of possible compiler
    //reordering optimization that causes seconds to get the original lat value
    s.append(QChar(0xB0)).append(" ")
     .append(QString::number(getCoordMinSec(lat))).append("' ");
    //lat is now seconds
    s.append(QString::fromStdString(getCoord(lat, 2))).append("\" ").append(c)
     .append(", ");
    c = (lon >= 0)? QObject::tr("E"):QObject::tr("W");
    if (lon < 0)
        lon = -lon;
    s.append(QString::number((int) lon)).append(QChar(0xB0)).append(" ");
    s.append(QString::number(getCoordMinSec(lon))).append("' ");
    s.append(QString::fromStdString(getCoord(lon, 2))).append("\" ").append(c);
    return s;
}

bool GisPoint::getAddress(double  lat,
                          double  lon,
                          string &addr,
                          string &state)
{
    bool retVal = false;
    DbInt::QResult *res = DbInt::instance().getAddress(getCoord(lat),
                                                       getCoord(lon));
    if (res == 0)
        return retVal;
    if (res->getNumRows() != 0)
    {
        //successful if can get state
        res->getFieldValue(DbInt::FIELD_DISTRICT, addr);
        retVal = res->getFieldValue(DbInt::FIELD_STATE, state);
    }
    delete res;
    return retVal;
}

double GisPoint::getLineDegree(double startLat,
                               double startLon,
                               double endLat,
                               double endLon)
{
    double degree = 0.0; //clockwise from north
    double xVal = endLon - startLon;
    double yVal = endLat - startLat;
    if (xVal == 0)
    {
        degree = (yVal > 0)? 0.0: 180.0;
    }
    else if (yVal == 0)
    {
        degree = (xVal > 0)? 90.0: 270.0;
    }
    else
    {
        degree = atan(abs(yVal)/abs(xVal)) * 180/PI;
        if (xVal < 0)
        {
            if (yVal > 0)
                degree += 270;
            else
                degree = 270 - degree;
        }
        else if (yVal < 0)
        {
            degree += 90;
        }
        else
        {
            degree = 90 - degree;
        }
    }
    return degree;
}
