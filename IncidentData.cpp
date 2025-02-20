/**
 * The incident data implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: IncidentData.cpp 1801 2024-01-04 08:14:26Z rosnin $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#include <QRegularExpression>

#include "QtUtils.h"
#include "Utils.h"
#include "IncidentData.h"

using std::map;
using std::string;

//invalid value for latitude and longitude - for initialization
static double       INVALID_LATLON = 999.0;
static const string BASE_CATEGORY_DEFAULT("default");

QStringList          IncidentData::sPriorityList;
QStringList          IncidentData::sEnPriorityList;
QStringList          IncidentData::sStateList;
QStringList          IncidentData::sAddrStateList;
map<QString, string> IncidentData::sCategoryMap;

IncidentData::IncidentData(int id, int priority, const QString &category) :
mId(id), mPriority(priority), mState(STATE_RECEIVED), mLockHolder(0),
mLatitude(INVALID_LATLON), mLongitude(INVALID_LATLON), mCategory(category)
{
}

#define SETDATETIME(field, dt, tm) \
    do \
    { \
        str = res->getFieldStr(DbInt::FIELD_INC_##field##_DATE, row); \
        qsl = QString::fromStdString(str).split(' ', \
                                                QString::SkipEmptyParts); \
        if (qsl.size() > 1) \
        { \
            dt = QDate::fromString(qsl.at(0), "dd/MM/yyyy"); \
            tm = QTime::fromString(qsl.at(1)); \
        } \
    } \
    while (0)

IncidentData::IncidentData(DbInt::QResult *res, int row) :
mId(0), mState(STATE_RECEIVED), mLockHolder(0), mLatitude(INVALID_LATLON),
mLongitude(INVALID_LATLON)
{
    if (res == 0 || row < 0 || row >= res->getNumRows())
    {
        assert("Bad param in IncidentData::IncidentData" == 0);
        return;
    }
    if (!res->getFieldValue(DbInt::FIELD_ID, mId, row) || mId <= 0)
        return;
    string str(res->getFieldStr(DbInt::FIELD_INC_LAT, row));
    if (!str.empty())
    {
        double lat = Utils::fromString<double>(str);
        str = res->getFieldStr(DbInt::FIELD_INC_LON, row);
        if (!str.empty())
        {
            mLatitude = lat;
            mLongitude = Utils::fromString<double>(str);
        }
    }
    setAddress(res->getFieldStr(DbInt::FIELD_INC_ADDR1, row),
               res->getFieldStr(DbInt::FIELD_INC_ADDR2, row),
               res->getFieldStr(DbInt::FIELD_STATE_DESC, row));
    mCallCardNum = QString::fromStdString(res->getFieldStr(
                                            DbInt::FIELD_INC_CALLCARD_NO, row));
    mPriority = getPriorityVal(QString::fromStdString(res->getFieldStr(
                                              DbInt::FIELD_INC_PRIORITY, row)));
    if (mPriority < 0)
        mPriority = getDefaultPriority();
    mCategory = QString::fromStdString(res->getFieldStr(
                                               DbInt::FIELD_INC_CATEGORY, row));
    mCreatedBy = QString::fromStdString(res->getFieldStr(
                                                 DbInt::FIELD_CREATED_BY, row));
    str = res->getFieldStr(DbInt::FIELD_MODIFIED_BY, row);
    if (str.empty())
    {
        setLastUpdateDateTime(QString::fromStdString(res->getFieldStr(
                                              DbInt::FIELD_CREATED_DATE, row)));
    }
    else
    {
        mUpdatedBy = QString::fromStdString(str);
        setLastUpdateDateTime(QString::fromStdString(res->getFieldStr(
                                             DbInt::FIELD_MODIFIED_DATE, row)));
    }
    mDescription = QString::fromStdString(res->getFieldStr(
                                                   DbInt::FIELD_INC_DESC, row));
    QStringList qsl;
    res->getFieldValue(DbInt::FIELD_INC_STATUS, mState, row);
    switch (mState)
    {
        case STATE_CLOSED:
            SETDATETIME(CLOSED, mCloseDate, mCloseTime);
            //fallthrough
        case STATE_ON_SCENE:
            SETDATETIME(ONSCENE, mOnSceneDate, mOnSceneTime);
            //fallthrough
        case STATE_DISPATCHED:
            SETDATETIME(DISP, mDispatchDate, mDispatchTime);
            //fallthrough
        case STATE_RECEIVED:
            SETDATETIME(RECVD, mCallRecvDate, mCallRecvTime);
            break;
        default:
            mId = 0;
            return;
    }
    string resources(res->getFieldStr(DbInt::FIELD_INC_RESOURCES, row));
    if (!resources.empty())
        mResources = QString::fromStdString(resources).split(",");
    res->getFieldValue(DbInt::FIELD_LOCK_HOLDER, mLockHolder, row);
}

void IncidentData::setAddress(const QString &addr1,
                              const QString &addr2,
                              const QString &state)
{
    mAddress1  = addr1;
    mAddress2  = addr2;
    mAddrState = state;
}

void IncidentData::setAddress(const string &addr1,
                              const string &addr2,
                              const string &state)
{
    mAddress1  = QString::fromStdString(addr1);
    mAddress2  = QString::fromStdString(addr2);
    mAddrState = QString::fromStdString(state);
}

void IncidentData::setState(int state, const QDate &date, const QTime &time)
{
    switch (state)
    {
        case STATE_RECEIVED:
            mCallRecvDate = date;
            mCallRecvTime = time;
            //fallthrough
        case STATE_DISPATCHED:
            mDispatchDate = date;
            mDispatchTime = time;
            //fallthrough
        case STATE_ON_SCENE:
            mOnSceneDate = date;
            mOnSceneTime = time;
            //fallthrough
        case STATE_CLOSED:
            mCloseDate = date;
            mCloseTime = time;
            break;
        default:
            assert("Invalid state in IncidentData::setState" == 0);
            return;
    }
    mState = state;
}

void IncidentData::setLocation(double lat, double lon)
{
    mLatitude = lat;
    mLongitude = lon;
}

bool IncidentData::getLocation(double &lat, double &lon) const
{
    if (!hasLocation())
        return false;
    lat = mLatitude;
    lon = mLongitude;
    return true;
}

bool IncidentData::hasLocation() const
{
    return (mLatitude != INVALID_LATLON && mLongitude != INVALID_LATLON);
}

const string &IncidentData::getBaseCat() const
{
    if (sCategoryMap.count(mCategory) != 0)
        return sCategoryMap[mCategory];
    return BASE_CATEGORY_DEFAULT;
}

void IncidentData::setLastUpdateDateTime(const QString &str)
{
    mUpdateDateTime = QtUtils::getDateTime(str);
}

QDate IncidentData::getDate(int state) const
{
    switch (state)
    {
        case STATE_CLOSED:
            return mCloseDate;
        case STATE_DISPATCHED:
            return mDispatchDate;
        case STATE_ON_SCENE:
            return mOnSceneDate;
        case STATE_RECEIVED:
            return mCallRecvDate;
        default:
            assert("Invalid state in IncidentData::getDate" == 0);
            return QDate::currentDate();
    }
}

QTime IncidentData::getTime(int state) const
{
    switch (state)
    {
        case STATE_CLOSED:
            return mCloseTime;
        case STATE_DISPATCHED:
            return mDispatchTime;
        case STATE_ON_SCENE:
            return mOnSceneTime;
        case STATE_RECEIVED:
            return mCallRecvTime;
        default:
            assert("Invalid state in IncidentData::getTime" == 0);
            return QTime::currentTime();
    }
}

void IncidentData::init()
{
    //caution: priority lists must have the same order as ePriority
    sEnPriorityList = QStringList() << "High" << "Normal" << "Low";
    sPriorityList = QStringList() << QObject::tr("High")
                                  << QObject::tr("Normal")
                                  << QObject::tr("Low");
    sStateList = QStringList() << QObject::tr("Opened")
                               << QObject::tr("Dispatched")
                               << QObject::tr("On-Scene");
}

int IncidentData::getPriorityVal(const QString &priority)
{
    int v = sPriorityList.indexOf(QRegularExpression("^" + priority + "$"));
    if (v >= 0)
        return v;
    //also cater for cases when DB data are not translated from english
    return sEnPriorityList.indexOf(QRegularExpression("^" + priority + "$"));
}

QString IncidentData::getStateText(int state)
{
    if (state >= STATE_RECEIVED && state <= STATE_ON_SCENE)
        return sStateList[state];
    return "";
}

int IncidentData::getStateVal(const QString &state)
{
    return sStateList.indexOf(QRegularExpression("^" + state + "$"));
}

void IncidentData::setCategories(const DbInt::DataMapT &data)
{
    for (auto &it : data)
    {
        sCategoryMap[QString::fromStdString(it.first)] = it.second;
    }
}

#ifdef NO_DB
void IncidentData::setCategories(const QStringList &data)
{
    for (const auto &s : data)
    {
        sCategoryMap[s] = s.toStdString();
    }
}
#endif

QStringList IncidentData::getCategoryList(bool base)
{
    QStringList l;
    if (base)
    {
        for (const auto &it : sCategoryMap)
        {
            l << QString::fromStdString(it.second);
        }
        l.sort();
    }
    else
    {
        for (const auto &it : sCategoryMap)
        {
            l << it.first;
        }
    }
    return l;
}

QString IncidentData::getBaseCategory(const QString &cat)
{
    if (sCategoryMap.count(cat) == 0)
        return QString::fromStdString(BASE_CATEGORY_DEFAULT);
    return QString::fromStdString(sCategoryMap[cat]);
}

void IncidentData::setAddrStates(const DbInt::DataMapT &data)
{
    sAddrStateList.clear();
    for (auto &it : data)
    {
        sAddrStateList << QString::fromStdString(it.first);
    }
}
