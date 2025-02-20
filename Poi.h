/**
 * The POI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2017. All Rights Reserved.
 *
 * @file
 * @version $Id: Poi.h 908 2017-03-10 02:07:21Z fashan $
 * @author Nur Zawanah Ishak
 */
#ifndef POI_H
#define POI_H

#include <map>

#include "Props.h"
#include "ServerSession.h"

class Poi : public QObject
{
public:
    /**
     * Constructor.
     *
     * @param[in] session Server session object.
     */
    Poi(ServerSession *session);

    /**
     * Loads POI from database.
     *
     * @param[out] prs   The POI collection.
     * @param[in]  uname The username.
     * @return true if successful.
     */
    bool loadData(Props::ValueMapsT &prs, const std::string &uname);

    /**
     * Loads a POI from database.
     *
     * @param[in,out] pr The POI properties.
     * @return true if successful.
     */
    bool loadData(Props::ValueMapT &pr);

    /**
     * Saves a POI to database.
     *
     * @param[in,out] pr The POI properties.
     * @return true if successful.
     */
    bool saveData(Props::ValueMapT &pr);

    /**
     * Deletes a POI from database.
     *
     * @param[in] id The POI ID.
     * @return true if successful.
     */
    bool deleteData(int id);

    void setSession(ServerSession *session) { mSession = session; }

private:
    ServerSession *mSession;

    /**
     * Converts a property field to database field.
     *
     * @param[in] fld Props::eField.
     * @return DbInt::eField.
     */
    int fieldPropsToDb(int fld);
};
#endif //POI_H
