/**
 * General purpose properties container with accessors and manipulators.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Props.h 1899 2025-01-22 05:42:07Z rosnin $
 * @author Nur Zawanah Ishak
 */
#ifndef PROPS_H
#define PROPS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "PalLock.h"
#include "Utils.h"

class Props
{
public:
    typedef std::map<int, std::string> ValueMapT;
    typedef std::vector<ValueMapT>     ValueMapsT;

    enum eField
    {
        FLD_UNDEFINED,

        //Settings
        FLD_CFG_AUDIO_IN,
        FLD_CFG_AUDIO_OUT,
        FLD_CFG_BRANCH,
        FLD_CFG_BRANCH_ALLOWED,
        FLD_CFG_CAMERA,
        FLD_CFG_CAMERA_RES,
        FLD_CFG_COLORTHEME,
        FLD_CFG_CONTACTS,
        FLD_CFG_GPS_MON,
        FLD_CFG_GRPCALL_AUTOJOIN,
        FLD_CFG_HELPDESKNUM,
        FLD_CFG_INC_ICONDIR,
        FLD_CFG_INCFILTER_ADDRSTATE,
        FLD_CFG_INCFILTER_CATEGORY,
        FLD_CFG_INCFILTER_PRIORITY,
        FLD_CFG_INCFILTER_STATE,
        FLD_CFG_LOGFILE,
        FLD_CFG_LOGLEVEL,
        FLD_CFG_MAP_CTR_RSC_CALL,
        FLD_CFG_MAP_MAXSCALE,
        FLD_CFG_MAP_SEA,
        FLD_CFG_MAP_TERM_LBL,
        FLD_CFG_MAP_TERM_STALE1,
        FLD_CFG_MAP_TERM_STALELAST,
        FLD_CFG_MMS_DOWNLOADDIR,
        FLD_CFG_MONITOR_RETAIN,
        FLD_CFG_MSG_TMR_INTERVAL,
        FLD_CFG_PTT_ALT,
        FLD_CFG_PTT_CHAR,
        FLD_CFG_PTT_CTRL,
        FLD_CFG_RSC_DSP_GRP,
        FLD_CFG_RSC_DSP_SUBS,
        FLD_CFG_SDSTEMPLATE,
        FLD_CFG_SERVERIP,
        FLD_CFG_SERVERPORT,

        //GIS
        FLD_COORDINATES,
        FLD_COORDINATES_MULTILINE,
        FLD_DISTANCE,
        FLD_FULLNAME,
        FLD_ICON,
        FLD_ID,
        FLD_IS_PUBLIC,
        FLD_ISSI,
        FLD_KEY,
        FLD_KEY_NEW,
        FLD_LAT,
        FLD_LAYERNAME,
        FLD_LBL,
        FLD_LOC_TIME_VALID,
        FLD_LOC_TIMESTAMP,
        FLD_LOC_TIMESTAMP_VALID,
        FLD_LON,
        FLD_OWNER,
        FLD_STATUS,
        FLD_TRAILING,
        FLD_TYPE,
        FLD_USERPOI_ADDR,
        FLD_USERPOI_CATEGORY,
        FLD_USERPOI_DESC,
        FLD_USERPOI_NAME,
        FLD_USERPOI_SHORTNAME
    };

    static const char VAL_COMBINER = '-';

    static const std::string VAL_INCIDENT_DEFTYPE;
    static const std::string VAL_ROUTE_START;
    static const std::string VAL_ROUTE_END;
    static const std::string VAL_TERMINAL_INVALID;
    static const std::string VAL_TERMINAL_VALID;
    static const std::string VAL_TERMINAL_STALE;

    /**
     * Gets the name for a field ID.
     *
     * @param[in] fld The field ID.
     * @return The field name, or "UNDEFINED" for invalid ID.
     */
    static const std::string &getFieldName(int fld);

    /**
     * Gets the ID for a field name.
     *
     * @param[in] name The field name.
     * @return The field ID. See eField. FLD_UNDEFINED for invalid name.
     */
    static int getFieldId(const std::string &name);

    /**
     * Converts properties to string, mainly for printing.
     *
     * @param[in] vals      The properties.
     * @param[in] pairDelim The field-value pair delimiter.
     * @param[in] delim     The delimiter between properties.
     * @return The string.
     */
    static std::string toString(const ValueMapT   &vals,
                                const std::string &pairDelim = "=",
                                const std::string &delim = ", ");

    /**
     * Sets a field value.
     *
     * @param[in] vals  The properties.
     * @param[in] fld   The field. See eField.
     * @param[in] value The value.
     */
    static void set(ValueMapT &vals, int fld, const std::string &value);

    /**
     * Sets a field value by combining 2 values with VAL_COMBINER, i.e.
     * <value1><VAL_COMBINER><value2>.
     *
     * @param[in] vals   The properties.
     * @param[in] fld    The field. See eField.
     * @param[in] value1 First value.
     * @param[in] value2 Second value.
     */
    static void set(ValueMapT         &vals,
                    int                fld,
                    const std::string &value1,
                    const std::string &value2);

    /**
     * Sets a field value by combining 2 source field values with
     * VAL_COMBINER, i.e. <fieldvalue1><VAL_COMBINER><fieldvalue2>.
     *
     * @param[in] vals The properties.
     * @param[in] fld  The field to set. See eField.
     * @param[in] src1 First source field.
     * @param[in] src2 Second source field.
     */
    static void set(ValueMapT &vals, int fld, int src1, int src2);

    /**
     * Sets a field value.
     *
     * @param[in] vals  The properties.
     * @param[in] fld   The field. See eField.
     * @param[in] value The value.
     * @tparam    T     The value type.
     */
    template<class T>
    static void set(ValueMapT &vals, int fld, T value);

    /**
     * Gets a field value.
     *
     * @param[in] vals The properties.
     * @param[in] fld  The field. See eField.
     * @return The value, or empty string if not found.
     */
    static const std::string &get(const ValueMapT &vals, int fld);

    /**
     * Gets a field value with a field name instead of ID, for special cases
     * only.
     *
     * @param[in] vals    The properties.
     * @param[in] fldName The field name.
     * @return The value, or empty string if not found.
     */
    static const std::string &get(const ValueMapT   &vals,
                                  const std::string &fldName);

    /**
     * Gets a field value. If not found, returns the specified default.
     *
     * @param[in] vals   The properties.
     * @param[in] fld    The field. See eField.
     * @param[in] defVal The default value.
     * @tparam    T      The value type.
     * @return The value. The default value if the field does not exist.
     */
    template<class T>
    static T get(const ValueMapT &vals, int fld, T defVal);

    /**
     * Gets a field value.
     *
     * @param[in] vals The properties.
     * @param[in] fld  The field. See eField.
     * @tparam    T    The value type.
     * @return The value. 0 or empty string if the field does not exist.
     */
    template<class T>
    static T get(const ValueMapT &vals, int fld);

    /**
     * Gets one part of a combined value using VAL_COMBINER.
     * E.g. for "xy<VAL_COMBINER>abcd", part 0 is "xy" and part 1 is "abcd".
     *
     * @param[in] vals The properties.
     * @param[in] fld  The field. See eField.
     * @param[in] idx  The 0-based part index.
     * @return The part, or empty string for invalid index.
     */
    static std::string getPart(const ValueMapT &vals, int fld, int idx);

    /**
     * Adds a string to the collection if it does not yet exist, and returns
     * its address. If it already exists, does nothing to the collection and
     * returns the existing address.
     * This allows multiple objects to share string values, similar to the
     * concept of string resources but with data added at runtime.
     * This should be used only for strings that are loaded once and never
     * modified nor removed, e.g. for static map attributes.
     * Before reloading the data, the user must call Props::clearStrs().
     *
     * @param[in] str The string.
     * @return The stored string address.
     */
    static const std::string *getStr(const std::string &str);

    /**
     * Removes a field.
     *
     * @param[in] vals The properties.
     * @param[in] fld  The field. See eField.
     */
    static void remove(ValueMapT &vals, int fld);

    /**
     * Clears the string collection.
     */
    static void clearStrs() { sVals.clear(); }

    /**
     * Adds field values from a source to a destination.
     *
     * @param[in] src The source.
     * @param[in] dst The destination.
     */
    static void add(const ValueMapT &src, ValueMapT &dst);

private:
    typedef std::set<std::string> ValsT;

    static ValueMapT      sFieldNames;
    static ValsT          sVals;       //shared strings collection
    static PalLock::LockT sValsLock;   //guards sVals insertions

    /**
     * Creates a mapping of field IDs to string. Used to initialize the
     * static map member.
     *
     * @return The map.
     */
    static ValueMapT createFieldNames();
};

template<class T>
void Props::set(ValueMapT &vals, int fld, T value)
{
    set(vals, fld, Utils::toString(value));
}

template<class T>
T Props::get(const ValueMapT &vals, int fld, T defVal)
{
    if (vals.count(fld) != 0)
        return Utils::fromString<T>(vals.at(fld));
    return defVal;
}

template<class T>
T Props::get(const ValueMapT &vals, int fld)
{
    return get(vals, fld, Utils::fromString<T>(""));
}
#endif //PROPS_H
