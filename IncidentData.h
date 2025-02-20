/**
 * The incident data module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: IncidentData.h 1801 2024-01-04 08:14:26Z rosnin $
 * @author Mazdiana Makmor
 */
#ifndef INCIDENTDATA_H
#define INCIDENTDATA_H

#include <QDate>
#include <QDateTime>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTime>

#include "DbInt.h"

class IncidentData
{
public:
    enum ePriority
    {
        PRIORITY_HIGH,
        PRIORITY_NORMAL,
        PRIORITY_LOW
    };

    enum eState
    {
        STATE_RECEIVED,
        STATE_DISPATCHED,
        STATE_ON_SCENE,
        STATE_CLOSED,
        STATE_OPEN
    };

    /**
     * Constructor.
     *
     * @param[in] id       The ID.
     * @param[in] priority The priority.
     * @param[in] category The category.
     */
    IncidentData(int id, int priority, const QString &category);

    /**
     * Constructs data from a database Incident search result.
     *
     * @param[in] res The result.
     * @param[in] row The row in the result.
     */
    IncidentData(DbInt::QResult *res, int row);

    bool isValid() const { return (mId > 0); }

    /**
     * Sets the address.
     *
     * @param[in] addr1 The address line 1.
     * @param[in] addr2 The address line 2.
     * @param[in] state The state.
     */
    void setAddress(const QString &addr1,
                    const QString &addr2,
                    const QString &state);

    /**
     * Sets the address.
     *
     * @param[in] addr1 The address line 1.
     * @param[in] addr2 The address line 2.
     * @param[in] state The state.
     */
    void setAddress(const std::string &addr1,
                    const std::string &addr2,
                    const std::string &state);

    QString getAddress1() const { return mAddress1; }

    QString getAddress2() const { return mAddress2; }

    QString getAddrState() const { return mAddrState; }

    /**
     * Sets the state, and the effective date and time.
     *
     * @param[in] state See eState.
     * @param[in] date  The date.
     * @param[in] time  The time.
     */
    void setState(int state, const QDate &date, const QTime &time);

    int getState() const { return mState; }

    bool isClosed() const { return (mState == STATE_CLOSED); }

    void setLockHolder(int holder) { mLockHolder = holder; }

    int getLockHolder() const { return mLockHolder; }

    bool isLocked() const { return (mLockHolder != 0); }

    /**
     * Sets the latitude and longitude of incident location.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void setLocation(double lat, double lon);

    /**
     * Gets the latitude and longitude of incident location.
     *
     * @param[out] lat The latitude.
     * @param[out] lon The longitude.
     * @return true if the location has been set, and outputs are valid.
     */
    bool getLocation(double &lat, double &lon) const;

    /**
     * Checks whether incident has latitude and longitude.
     *
     * @return true if it does.
     */
    bool hasLocation() const;

    double getLat() const { return mLatitude; }

    double getLon() const { return mLongitude; }

    void setCallCardNum(const QString &num) { mCallCardNum = num; }

    const QString &getCallCardNum() const { return mCallCardNum; }

    void setDescription(const QString &desc) { mDescription = desc; }

    const QString &getDescription() const { return mDescription; }

    void setCreatedBy(const QString &str) { mCreatedBy = str; }

    const QString &getCreatedBy() const { return mCreatedBy; }

    void setUpdatedBy(const QString &updatedBy) { mUpdatedBy = updatedBy; }

    const QString &getUpdatedBy() const { return mUpdatedBy; }

    const QDateTime &getLastUpdateDateTime() const { return mUpdateDateTime; }

    int getId() const { return mId; }

    void setCategory(const QString &str) { mCategory = str; }

    const QString &getCategory() const { return mCategory; }

    const std::string &getBaseCat() const;

    void setPriority(int val) { mPriority = val; }

    int getPriority() const { return mPriority; }

    void setResources(const QStringList &list) { mResources = list; }

    QStringList getResources() const { return mResources; }

    /**
     * Sets the last update time.
     *
     * @param[in] str The time in "YYYY-MM-dd hh:mm:ss" format.
     */
    void setLastUpdateDateTime(const QString &str);

    /**
     * Gets the effective date for the given state.
     *
     * @param[in] state The state.
     * @return The date.
     */
    QDate getDate(int state) const;

    /**
     * Gets the effective time for the given state.
     *
     * @param[in] state The state.
     * @return The time.
     */
    QTime getTime(int state) const;

    bool isHighPriority() const { return (mPriority == PRIORITY_HIGH); }

    /**
     * Initializes static members that are subject to text translation.
     */
    static void init();

    static int getDefaultPriority() { return PRIORITY_NORMAL; }

    static QStringList getPriorityList() { return sPriorityList; }

    /**
     * Converts a priority name to value.
     *
     * @param[in] priority The priority.
     * @return The value, or -1 for invalid name.
     */
    static int getPriorityVal(const QString &priority);

    static QStringList getStateList() { return sStateList; }

    /**
     * Converts a state value to name.
     *
     * @param[in] state The state value.
     * @return The name, or empty string for invalid value.
     */
    static QString getStateText(int state);

    /**
     * Converts a state name to value.
     *
     * @param[in] state The state name.
     * @return The value, or -1 for invalid name.
     */
    static int getStateVal(const QString &state);

    /**
     * Sets the categories.
     *
     * @param[in] data The category map.
     */
    static void setCategories(const DbInt::DataMapT &data);

#ifdef NO_DB
    /**
     * Sets the categories.
     *
     * @param[in] data The category list.
     */
    static void setCategories(const QStringList &data);
#endif

    /**
     * Gets the categories.
     *
     * @param[in] base true to get base categories instead of displayed ones.
     * @return The category list.
     */
    static QStringList getCategoryList(bool base = false);

    /**
     * Gets the base category for a displayed category.
     *
     * @param[in] cat The category.
     * @return The base category.
     */
    static QString getBaseCategory(const QString &cat);

    /**
     * Sets the address states.
     *
     * @param[in] data The address state map.
     */
    static void setAddrStates(const DbInt::DataMapT &data);

    static QStringList getAddrStateList() { return sAddrStateList; }

private:
    int         mId;
    int         mPriority;
    int         mState;
    int         mLockHolder;
    double      mLatitude;
    double      mLongitude;
    QDate       mCallRecvDate;
    QDate       mCloseDate;
    QDate       mDispatchDate;
    QDate       mOnSceneDate;
    QTime       mCallRecvTime;
    QTime       mCloseTime;
    QTime       mDispatchTime;
    QTime       mOnSceneTime;
    QString     mCallCardNum;
    QString     mAddress1;
    QString     mAddress2;
    QString     mAddrState;
    QString     mCategory;
    QString     mCreatedBy;
    QString     mDescription;
    QString     mUpdatedBy;
    QDateTime   mUpdateDateTime;
    QStringList mResources;

    static QStringList sPriorityList;
    static QStringList sEnPriorityList;
    static QStringList sStateList;
    static QStringList sAddrStateList;
    //key=category, value=base category
    static std::map<QString, std::string> sCategoryMap;
};
//enable use as table data
Q_DECLARE_METATYPE(IncidentData *)
#endif //INCIDENTDATA_H
