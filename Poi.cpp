/**
 * The POI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2019. All Rights Reserved.
 *
 * @file
 * @version $Id: Poi.cpp 1240 2019-05-03 08:09:00Z zulzaidi $
 * @author Nur Zawanah Ishak
 */
#include <assert.h>
#include <QString>

#include "DbInt.h"
#include "Poi.h"

using namespace std;

Poi::Poi(ServerSession *session) : mSession(session)
{
    if (session == 0)
        assert("Bad param in Poi::Poi" == 0);
}

#define SETPR(fdb, fp) \
    do \
    { \
        if (!res->getFieldValue(DbInt::FIELD_##fdb, val, i)) \
            continue; \
        Props::set(pr, Props::FLD_##fp, val); \
    } \
    while (0)

#define SETPRBOOL(fdb, fp) \
    do \
    { \
        if (!res->getFieldValue(DbInt::FIELD_##fdb, val, i)) \
            continue; \
        Props::set(pr, Props::FLD_##fp, \
                   res->getFieldBool(DbInt::FIELD_##fdb, i)); \
    } \
    while (0)

bool Poi::loadData(Props::ValueMapsT &prs, const string &uname)
{
    DbInt::QResult *res = DbInt::instance().getPoi(uname);
    if (res == 0)
        return false;
    string val;
    Props::ValueMapT pr;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        pr.clear();
        SETPR(ID, KEY);
        SETPR(NAME, USERPOI_NAME);
        SETPR(SHORTNAME, USERPOI_SHORTNAME);
        SETPR(LATITUDE, LAT);
        SETPR(LONGITUDE, LON);
        SETPR(ADDRESS, USERPOI_ADDR);
        SETPR(CATEGORY, USERPOI_CATEGORY);
        SETPR(DESC, USERPOI_DESC);
        SETPR(OWNER, OWNER);
        SETPRBOOL(IS_PUBLIC, IS_PUBLIC);
        prs.push_back(pr);
    }
    delete res;
    return true;
}

#define SETPR0(fdb, fp) \
    do \
    { \
        if (res->getFieldValue(DbInt::FIELD_##fdb, val)) \
            Props::set(pr, Props::FLD_##fp, val); \
    } \
    while (0)

#define SETPR0BOOL(fdb, fp) \
    do \
    { \
        if (res->getFieldValue(DbInt::FIELD_##fdb, val)) \
            Props::set(pr, Props::FLD_##fp, \
                       res->getFieldBool(DbInt::FIELD_##fdb)); \
    } \
    while (0)

bool Poi::loadData(Props::ValueMapT &pr)
{
    DbInt::QResult *res = DbInt::instance()
                              .getPoi(Props::get<int>(pr, Props::FLD_KEY));
    if (res == 0)
        return false;
    string val;
    SETPR0(NAME, USERPOI_NAME);
    SETPR0(SHORTNAME, USERPOI_SHORTNAME);
    SETPR0(LATITUDE, LAT);
    SETPR0(LONGITUDE, LON);
    SETPR0(ADDRESS, USERPOI_ADDR);
    SETPR0(CATEGORY, USERPOI_CATEGORY);
    SETPR0(DESC, USERPOI_DESC);
    SETPR0(OWNER, OWNER);
    SETPR0BOOL(IS_PUBLIC, IS_PUBLIC);
    delete res;
    return true;
}

bool Poi::saveData(Props::ValueMapT &pr)
{
#ifndef NO_DB
    DbInt::RecordT rec;
    Props::ValueMapT::const_iterator it = pr.begin();
    for (; it!=pr.end(); ++it)
    {
        rec[fieldPropsToDb(it->first)] = it->second;
    }
    if (rec.count(DbInt::FIELD_LONGITUDE) != 0 &&
        rec.count(DbInt::FIELD_LATITUDE) != 0)
        rec[DbInt::FIELD_SHAPE].assign(rec[DbInt::FIELD_LONGITUDE])
                               .append(" ")
                               .append(rec[DbInt::FIELD_LATITUDE]);
    int id = Props::get<int>(pr, Props::FLD_KEY);
    if (id < 0)
    {
        if (rec.count(DbInt::FIELD_ID) != 0)
            rec.erase(DbInt::FIELD_ID);
        if (!DbInt::instance().insertRecord(DbInt::DB_TABLE_D_POI, rec, &id))
            return false;
        Props::set(pr, Props::FLD_KEY_NEW, id);
    }
    else if (!DbInt::instance().updateRecord(DbInt::DB_TABLE_D_POI,
                                             DbInt::FIELD_ID,
                                             Props::get(pr, Props::FLD_KEY),
                                             rec))
    {
        return false;
    }
    if (pr.count(Props::FLD_IS_PUBLIC) != 0)
        mSession->poiUpdate(id, !Props::get<bool>(pr, Props::FLD_IS_PUBLIC));
#endif //NO_DB
    return true;
}

bool Poi::deleteData(int id)
{
#ifndef NO_DB
    DbInt::RecordT rec;
    rec[DbInt::FIELD_ID] = Utils::toString(id);
    if (!DbInt::instance().deleteRecord(DbInt::DB_TABLE_D_POI, rec))
        return false;
    mSession->poiUpdate(id, true);
#endif
    return true;
}

int Poi::fieldPropsToDb(int fld)
{
    switch (fld)
    {
        case Props::FLD_IS_PUBLIC:
            return DbInt::FIELD_IS_PUBLIC;
        case Props::FLD_KEY:
            return DbInt::FIELD_ID;
        case Props::FLD_LAT:
            return DbInt::FIELD_LATITUDE;
        case Props::FLD_LON:
            return DbInt::FIELD_LONGITUDE;
        case Props::FLD_OWNER:
            return DbInt::FIELD_OWNER;
        case Props::FLD_USERPOI_ADDR:
            return DbInt::FIELD_ADDRESS;
        case Props::FLD_USERPOI_CATEGORY:
            return DbInt::FIELD_CATEGORY;
        case Props::FLD_USERPOI_DESC:
            return DbInt::FIELD_DESC;
        case Props::FLD_USERPOI_NAME:
            return DbInt::FIELD_NAME;
        case Props::FLD_USERPOI_SHORTNAME:
            return DbInt::FIELD_SHORTNAME;
    }
    return DbInt::FIELD_UNDEFINED;
}
