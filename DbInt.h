/**
 * A singleton class for database operation functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: DbInt.h 1909 2025-03-06 08:06:00Z hazim.rujhan $
 * @author Muhd Hashim Wahab
 */
#ifndef DBINT_H
#define DBINT_H

#include <map>
#include <set>
#include <string>
#include <vector>
#ifndef NO_DB
#include <libpq-fe.h>
#endif

#include "Logger.h"
#include "PalLock.h"
#include "PalThread.h"

class DbInt
{
public:
#ifdef NO_DB
    typedef int PGconn;     //dummy
    typedef int PGresult;
#endif
    typedef PGconn                             DbConnT;
    typedef PGresult                           QueryResultT;
    typedef std::map<int, std::string>         RecordT;
    typedef std::map<std::string, std::string> DataMapT;
    typedef std::map<int, std::string>         Int2StringMapT;

    enum
    {
        //Postgresql Oid type. To get the full list, execute
        //"SELECT typname, oid FROM pg_type;" from postgres database.
        PARAMTYPE_BOOL = 16,     //boolean
        PARAMTYPE_CHAR = 18,     //character
        PARAMTYPE_INT8 = 20,     //big integer
        PARAMTYPE_INT2 = 21,     //tiny integer
        PARAMTYPE_INT4 = 23,     //integer
        PARAMTYPE_TEXT = 25,     //string
        PARAMTYPE_OID  = 26      //pg_type
    };

    enum eField
    {
        FIELD_ADDRESS,
        FIELD_AUDIO_PATH,
        FIELD_AUDIT_ACTION,
        FIELD_AUDIT_DATE,
        FIELD_AUDIT_DESC,
        FIELD_BASE,
        FIELD_BR_BRANCH,
        FIELD_BR_CLUSTER,
        FIELD_BR_DISP,
        FIELD_BR_EM_SSI,
        FIELD_BR_MOBILE,
        FIELD_BR_SVR,
        FIELD_CALL_DURATION,
        FIELD_CALL_HOOK_METHOD,
        FIELD_CALL_ID,
        FIELD_CALL_SIMPLEX_DUPLEX,
        FIELD_CALL_TYPE,
        FIELD_CATEGORY,
        FIELD_COORDS,
        FIELD_CREATED_BY,
        FIELD_CREATED_DATE,
        FIELD_DATETIME,
        FIELD_DESC,
        FIELD_DISC_CAUSE,
        FIELD_DISTANCE,
        FIELD_DISTRICT,
        FIELD_FILENAME,
        FIELD_FLEET,
        FIELD_FROM,
        FIELD_FROM_TYPE,
        FIELD_GSSI,
        FIELD_ID,
        FIELD_IH_ID,
        FIELD_INC_ADDR1,
        FIELD_INC_ADDR2,
        FIELD_INC_CALLCARD_NO,
        FIELD_INC_CATEGORY,
        FIELD_INC_CLOSED_DATE,
        FIELD_INC_DESC,
        FIELD_INC_DISP_DATE,
        FIELD_INC_LAT,
        FIELD_INC_LON,
        FIELD_INC_ONSCENE_DATE,
        FIELD_INC_PRIORITY,
        FIELD_INC_RECVD_DATE,
        FIELD_INC_RESOURCES,
        FIELD_INC_STATE,
        FIELD_INC_STATUS,
        FIELD_IR_ISSI,
        FIELD_IS_PUBLIC,
        FIELD_ISSI,
        FIELD_ITEMCAT,
        FIELD_LATITUDE,
        FIELD_LAYER,
        FIELD_LOC_ISSI,
        FIELD_LOC_LATLONG,
        FIELD_LOC_TIME,
        FIELD_LOCK_HOLDER,
        FIELD_LONGITUDE,
        FIELD_MODIFIED_BY,
        FIELD_MODIFIED_DATE,
        FIELD_MON,
        FIELD_NAME,
        FIELD_NOTES,
        FIELD_OWNER,
        FIELD_RTE_COST,
        FIELD_RTE_MULTILINE,
        FIELD_SDS_MSG,
        FIELD_SHAPE,
        FIELD_SHORTNAME,
        FIELD_SSI,
        FIELD_START,
        FIELD_STATE,
        FIELD_STATE_CODE,
        FIELD_STATE_DESC,
        FIELD_STATUSCODE_CODE,
        FIELD_STATUSCODE_TEXT,
        FIELD_STATUSCODE_TYPE,
        FIELD_SUBDISTRICT,
        FIELD_TEXT,
        FIELD_TIME,
        FIELD_TO,
        FIELD_TO_TYPE,
        FIELD_TX_PARTY,
        FIELD_TX_PARTY_TYPE,
        FIELD_TYPE,
        FIELD_TYPE_DESC,
        FIELD_USER_ACTIVE,
        FIELD_USER_FAILED_COUNT,
        FIELD_USER_FLEET,
        FIELD_USER_GROUP_ID,
        FIELD_USER_GROUP_NAME,
        FIELD_USER_NAME,
        FIELD_USER_PASSWD,
        FIELD_USER_ROLE,
        FIELD_VOIP_ID,
        FIELD_ZOOM,
        FIELD_UNDEFINED
    };

    enum eAction
    {
        ACT_ADD_GRPDATA,
        ACT_ADD_INCIDENT,
        ACT_ADD_STSMSG,
        ACT_ADD_SUBSDATA,
        ACT_ADD_USER_CLIENT,
        ACT_ADD_USER_MOBILE,
        ACT_ADD_USER_SERVER,
        ACT_CFG,
#ifdef AGW
        ACT_AGW_RESTART,
        ACT_CFG_AGW_ISSI,
        ACT_CFG_AGW_LOGIN,
        ACT_CFG_AGW_MONSSIS,
        ACT_CFG_AGW_SERVERIP,
        ACT_CFG_AGW_SERVERPORT,
        ACT_CFG_AGW_SERVERSITENAME,
#endif
        ACT_CFG_ALERT_SERVERIP,
        ACT_CFG_ALERT_SERVERPORT,
        ACT_CFG_DB_ADDRESS,
        ACT_CFG_DB_NAME,
        ACT_CFG_DB_PASSWORD,
        ACT_CFG_DB_PORT,
        ACT_CFG_DB_USERNAME,
        ACT_CFG_EM_SSI,
        ACT_CFG_HELPDESKNUM,
        ACT_CFG_KEEPALIVE_PERIOD,
        ACT_CFG_LOGFILE,
        ACT_CFG_LOGLEVEL,
        ACT_CFG_LOGMAXCOUNT,
        ACT_CFG_LOGMAXSIZE,
        ACT_CFG_MAXCONNATTEMPTS,
        ACT_CFG_PEER_IP,
        ACT_CFG_PEER_PORT,
        ACT_CFG_PORT,
        ACT_CFG_REDUNDANCY_IP,
        ACT_CFG_SAVE,
        ACT_CFG_SDM_IP,
        ACT_CFG_SDM_PORT,
        ACT_CFG_SDM_RESTART,
        ACT_CFG_STM_ID,
        ACT_CFG_STM_IP,
        ACT_CFG_STM_PORT,
        ACT_CFG_STM_RESTART,
        ACT_CFG_TDP_PASSWORD,
        ACT_CFG_TDP_SERVERIP,
        ACT_CFG_TDP_SERVERPORT,
        ACT_CFG_TDP_USERNAME,
        ACT_CFG_TDP_USERNAMEGPS,
        ACT_CFG_TRANS_TIMEOUT,
        ACT_CFG_VOIP_SERVERIP,
        ACT_CFG_VOIP_SERVERPORT,
        ACT_DEL_GRPDATA,
        ACT_DEL_STSMSG,
        ACT_DEL_SUBSDATA,
        ACT_DEL_USER_CLIENT,
        ACT_DEL_USER_MOBILE,
        ACT_DEL_USER_SERVER,
        ACT_EDIT_GRPDATA,
        ACT_EDIT_INCIDENT,
        ACT_EDIT_STSMSG,
        ACT_EDIT_SUBSDATA,
        ACT_EDIT_USER_CLIENT,
        ACT_EDIT_USER_MOBILE,
        ACT_EDIT_USER_SERVER,
        ACT_LOGIN_CLIENT,
        ACT_LOGIN_SERVER,
        ACT_LOGOUT_CLIENT,
        ACT_LOGOUT_SERVER,
        ACT_RESTART_TDPSESSION,
        ACT_SERVER_START,
        ACT_SERVER_STOP
    };

    enum eUserType
    {
        USERTYPE_SERVER,  //must be 0
        USERTYPE_CLIENT,
        USERTYPE_MOBILE,
        USERTYPE_INVALID  //for validation
    };

    struct StatusData
    {
        int         code;
        int         type;
        std::string text;
    };

    static const std::string DB_TABLE_D_BOOKMARK;
    static const std::string DB_TABLE_D_CALL_AUDIO;
    static const std::string DB_TABLE_D_CALL_HIST;
    static const std::string DB_TABLE_D_CONTACTS;
    static const std::string DB_TABLE_D_COUNTRY_STATE;
    static const std::string DB_TABLE_D_GPS;
    static const std::string DB_TABLE_D_INC_CATEGORY;
    static const std::string DB_TABLE_D_INC_HIST;
    static const std::string DB_TABLE_D_INC_RSC;
    static const std::string DB_TABLE_D_MMS_HIST;
    static const std::string DB_TABLE_D_PHONEBOOK;
    static const std::string DB_TABLE_D_POI;
    static const std::string DB_TABLE_D_POI_CATEGORY;
    static const std::string DB_TABLE_D_PTT_HIST;
    static const std::string DB_TABLE_D_SDS_HIST;
    static const std::string DB_TABLE_D_STSMSG;
    static const std::string DB_TABLE_D_STSMSG_HIST;
    static const std::string DB_TABLE_D_STSMSG_TYPE;
    static const std::string DB_TABLE_D_USER_CLIENT;
    static const std::string DB_TABLE_D_USER_CLIENT_DISP;
    static const std::string DB_TABLE_D_USER_GROUP;
    static const std::string DB_TABLE_D_USER_SERVER;

    class QResult
    {
    public:
        /**
         * Constructor.
         *
         * @param[in] res The query result.
         */
        QResult(QueryResultT *res);

        ~QResult();

        int getNumRows();

        /**
         * Checks if field exists with a value.
         *
         * @param[in] field The field.
         * @param[in] row   The 0-based row number.
         * @return true if exists.
         */
        bool hasField(int field, int row = 0);

        /**
         * Gets the string from a specific field.
         *
         * @param[in]  field The field. See eField.
         * @param[out] value The field value, unmodified on failure.
         * @param[in]  row   The 0-based row number.
         * @return true if successful.
         */
        bool getFieldValue(int field, std::string &value, int row = 0);

        /**
         * Gets the value from a specific field.
         *
         * @param[in]  field The field. See eField.
         * @param[out] value The field value, unmodified on failure.
         * @param[in]  row   The 0-based row number.
         * @return true if successful.
         */
        bool getFieldValue(int field, int &value, int row = 0);

        /**
         * Gets the string value from a specific field.
         *
         * @param[in] field The field. See eField.
         * @param[in] row   The 0-based row number.
         * @return The field value, or empty string if field not found.
         */
        std::string getFieldStr(int field, int row = 0);

        /**
         * Gets the boolean value from a specific field.
         *
         * @param[in] field The field. See eField.
         * @param[in] row   The 0-based row number.
         * @return The field boolean value, or false if field not found.
         */
        bool getFieldBool(int field, int row = 0);

        /**
         * Gets the positive integer return value.
         *
         * @return The value, or -1 on failure.
         */
        int getReturnInt();

    private:
        QueryResultT *mResult;
    }; //class QResult

    /**
     * Gets the object validity. If invalid, tries to restore and returns the
     * resulting status.
     *
     * @param[in] chkOnly true to skip restore.
     * @return true if valid.
     */
    bool isValid(bool chkOnly = false);

    /**
     * Runs a query with no parameter.
     *
     * @param[in] query The query.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *queryExec(const std::string &query);

    /**
     * Runs a query with parameters.
     * By default, all inputs are considered as text. If parameter is other
     * than text, paramTypes must be provided.
     * Quoting/escaping is not needed when assigning parameter values.
     * E.g. "joe's place" instead of "joe''s place".
     *
     * @param[in] query       The query.
     * @param[in] paramValues The parameter values.
     * @param[in] paramTypes  The parameter types.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *queryExec(const std::string        &query,
                       std::vector<std::string> &paramValues,
                       const unsigned int       *paramTypes = 0);

    /**
     * Runs a command.
     *
     * @param[in] cmd The command.
     * @return true if successful.
     */
    bool commandExec(const std::string &cmd);

    /**
     * Gets a string field value using another (key) field value.
     *
     * @param[in] table    The table name.
     * @param[in] field    The field. See eField.
     * @param[in] keyField The key field. See eField.
     * @param[in] keyValue The key value.
     * @return The field value. Empty string if not found.
     */
    std::string getStr(const std::string &table,
                       int                field,
                       int                keyField,
                       const std::string &keyValue);

    /**
     * Gets an integer field value using another (key) field value.
     *
     * @param[in] table    The table name.
     * @param[in] field    The field. See eField.
     * @param[in] keyField The key field. See eField.
     * @param[in] keyValue The key value.
     * @return The field value. -1 if not found.
     */
    int getValue(const std::string &table,
                 int                field,
                 int                keyField,
                 const std::string &keyValue);

    /**
     * Gets rows for the given field name and value.
     *
     * @param[in] table    The table name.
     * @param[in] field    The field. See eField.
     * @param[in] keyValue The key value.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getRows(const std::string &table,
                     int                field,
                     const std::string &keyValue);

    /**
     * Gets fleet branches.
     *
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_BR_BRANCH
     *         -FIELD_BR_CLUSTER
     *         -FIELD_NAME
     *         -FIELD_SSI
     *         -FIELD_BR_DISP
     *         -FIELD_BR_EM_SSI
     *         -FIELD_BR_MOBILE
     *         -FIELD_BR_SVR
     */
    QResult *getBranches();

    /**
     * Saves a branch.
     *
     * @param[in] branch  The branch.
     * @param[in] cluster The cluster ID.
     * @param[in] name    The name.
     * @return true if successful.
     */
    bool saveBranch(int branch, int cluster, const std::string &name);

    /**
     * Deletes a branch.
     *
     * @param[in] branch The branch.
     * @return true if successful.
     */
    bool deleteBranch(int branch);

    /**
     * Saves branch IDs.
     *
     * @param[in] branch The branch.
     * @param[in] field  The field. One of:
     *                   -FIELD_SSI
     *                   -FIELD_BR_DISP
     *                   -FIELD_BR_EM_SSI
     *                   -FIELD_BR_MOBILE
     *                   -FIELD_BR_SVR
     * @param[in] ids    The IDs.
     * @return true if successful.
     */
    bool saveBranchIds(int branch, int field, const std::string &ids);

    /**
     * Gets subscriber data.
     *
     * @param[in] ind true for individual, otherwise group.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getSubsData(bool ind);

    /**
     * Saves a subscriber data.
     *
     * @param[in] ind  true for individual, otherwise group.
     * @param[in] ssi  The ISSI or GSSI.
     * @param[in] type The type.
     * @param[in] desc The description.
     * @return true if successful.
     */
    bool saveSubsData(bool               ind,
                      const std::string &ssi,
                      const std::string &type,
                      const std::string &desc);

    /**
     * Deletes a Subscriber Data.
     *
     * @param[in] ind true for individual, otherwise group.
     * @param[in] ssi The ISSI or GSSI.
     * @return true if successful.
     */
    bool deleteSubsData(bool ind, const std::string &ssi);

    /**
     * Gets terminal types.
     *
     * @param[out] tTypes The terminal types.
     * @return true if successful.
     */
    bool getTerminalTypes(Int2StringMapT &tTypes);

    /**
     * Gets Status Message types.
     *
     * @param[out] smTypes The Status Message types.
     * @return true if successful.
     */
    bool getStatusTypes(Int2StringMapT &smTypes);

    /**
     * Gets status codes.
     *
     * @param[out] scList The status codes.
     * @param[in]  fleet  Fleet ID. Only used if non-negative in STM-nwk.
     * @return true if successful.
     */
    bool getStatusCodes(std::vector<StatusData> &scList, int fleet = -1);

    /**
     * Gets the country states.
     *
     * @param[out] states The states: name => code.
     * @return true if successful.
     */
    bool getCountryStates(DataMapT &states);

    /**
     * Gets Quick Contact entries for a user.
     *
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_ID
     *         -FIELD_TYPE
     *         -FIELD_NOTES
     *         -FIELD_DESCR
     *         -FIELD_MON
     */
    QResult *getContacts(const std::string &username);

    /**
     * Gets phonebook entries for a user.
     *
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TYPE
     *         -FIELD_SSI
     *         -FIELD_NOTES
     */
    QResult *getPhonebook(const std::string &username);

    /**
     * Gets incident categories.
     *
     * @param[out] categories The categories: category => base category.
     * @return true if successful.
     */
    bool getIncidentCategories(DataMapT &categories);

    /**
     * Gets all incidents with a status not matching the given status.
     *
     * @param[in] exclStatus The status value to exclude.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getIncidentsByExclStatus(int exclStatus);

    /**
     * Locks or unlocks an incident for editing.
     *
     * @param[in] id     The incident ID.
     * @param[in] holder The user ID to lock, or 0 to unlock.
     * @return The holder if successful, or another user ID if the incident was
     *         already locked, or -1 on error.
     */
    int lockIncident(int id, int holder);

    /**
     * Gets the incident ID locked by a user.
     *
     * @param[in] username The username.
     * @return The incident ID if successful, or 0 if none, or -1 on error.
     */
    int getLockedIncident(const std::string &username);

    /**
     * Creates an incident.
     *
     * @param[in] record    The incident record.
     * @param[in] resources The resource ISSIs. May be empty.
     * @return The incident ID if successful, or -1 on error.
     */
    int createIncident(const RecordT               &record,
                       const std::set<std::string> &resources);

    /**
     * Updates an incident.
     *
     * @param[in] id        The incident ID.
     * @param[in] record    The updated record.
     * @param[in] resources The resource ISSIs. May be empty.
     * @return true if successful.
     */
    bool updateIncident(const std::string           &id,
                        const RecordT               &record,
                        const std::set<std::string> &resources);

    /**
     * Gets the call history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] type      The call type or empty string for all types.
     * @param[in] from      The from ID or 0 for all.
     * @param[in] to        The to ID or 0 for all.
     * @param[in] doAnd     true to AND the 'from' and 'to', false to OR.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TIME
     *         -FIELD_FROM
     *         -FIELD_TO
     *         -FIELD_TO_TYPE
     *         -FIELD_CALL_DURATION
     *         -FIELD_CALL_ID
     *         -FIELD_CALL_SIMPLEX_DUPLEX
     *         -FIELD_CALL_TYPE
     *         -FIELD_AUDIO_PATH
     */
    QResult *getCallHistory(const std::string &startTime,
                            const std::string &endTime,
                            const std::string &type,
                            int                from,
                            int                to,
                            bool               doAnd);

    /**
     * Gets the last call entry.
     *
     * @param[in] from The calling party ID.
     * @param[in] to   The called party ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLastCall(int from, int to);

    /**
     * Gets SDS history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] msg       The message keywords in one of the following
     *                      formats:
     *                      -Space separated words for any occurence of either
     *                       word (case-insensitive).
     *                      -Quoted string for any occurence of a phrase
     *                       (case-insensitive).
     *                      -Empty if not applicable.
     * @param[in] from      The from ID or 0 for all.
     * @param[in] to        The to ID or 0 for all.
     * @param[in] doAnd     true to AND the 'from' and 'to', false to OR.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TIME
     *         -FIELD_FROM
     *         -FIELD_TO
     *         -FIELD_TO_TYPE
     *         -FIELD_SDS_MSG
     */
    QResult *getSdsHistory(const std::string &startTime,
                           const std::string &endTime,
                           const std::string &msg,
                           int                from,
                           int                to,
                           bool               doAnd);

    /**
     * Gets the last SDS entry.
     *
     * @param[in] from The sender ID.
     * @param[in] to   The recipient ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLastSds(int from, int to);

    /**
     * Gets Status Message history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] text      The text keywords in one of the following
     *                      formats:
     *                      -Space separated words for any occurence of either
     *                       word (case-insensitive).
     *                      -Quoted string for any occurence of a phrase
     *                       (case-insensitive).
     *                      -Empty if not applicable.
     * @param[in] from      The from ID or 0 for all.
     * @param[in] to        The to ID or 0 for all.
     * @param[in] doAnd     true to AND the 'from' and 'to', false to OR.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TIME
     *         -FIELD_FROM
     *         -FIELD_TO
     *         -FIELD_TO_TYPE
     *         -FIELD_STATUSCODE_CODE
     *         -FIELD_STATUSCODE_TEXT
     */
    QResult *getStsMsgHistory(const std::string &startTime,
                              const std::string &endTime,
                              const std::string &text,
                              int                from,
                              int                to,
                              bool               doAnd);

    /**
     * Gets the last Status Message entry.
     *
     * @param[in] from The sender ID.
     * @param[in] to   The recipient ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLastSts(int from, int to);

    /**
     * Gets MMS history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] msg       The message keywords in one of the following
     *                      formats:
     *                      -Space separated words for any occurence of either
     *                       word (case-insensitive).
     *                      -Quoted string for any occurence of a phrase
     *                       (case-insensitive).
     *                      -Empty if not applicable.
     * @param[in] from      The from ID or 0 for all.
     * @param[in] to        The to ID or 0 for all.
     * @param[in] doAnd     true to AND the 'from' and 'to', false to OR.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TIME
     *         -FIELD_FROM
     *         -FIELD_FROM_TYPE
     *         -FIELD_TO
     *         -FIELD_TO_TYPE
     *         -FIELD_TEXT
     *         -FIELD_FILENAME
     */
    QResult *getMmsHistory(const std::string &startTime,
                           const std::string &endTime,
                           const std::string &msg,
                           int                from,
                           int                to,
                           bool               doAnd);

    /**
     * Gets the last MMS entry.
     *
     * @param[in] from The sender ID.
     * @param[in] to   The recipient ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLastMms(int from, int to);

    /**
     * Gets MMS, SDS and Status Message history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] from      The from ID or 0 for all.
     * @param[in] to        The to ID or 0 for all.
     * @param[in] doAnd     true to AND the 'from' and 'to', false to OR.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     *         If successful, contains fields:
     *         -FIELD_TIME
     *         -FIELD_FROM
     *         -FIELD_TO
     *         -FIELD_TO_TYPE
     *         -FIELD_STATUSCODE_CODE (value is always null for MMS and SDS)
     *         -FIELD_SDS_MSG (contain MMS/Status text for MMS/Status Message)
     *         -FIELD_FILENAME (value is always null for SDS and Status Message)
     */
    QResult *getMsgHistory(const std::string &startTime,
                           const std::string &endTime,
                           int                from,
                           int                to,
                           bool               doAnd);

    /**
     * Gets an Incident history entry.
     *
     * @param[in] id The ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getIncidentHistory(int id);

    /**
     * Gets Incident history entries.
     *
     * @param[in] startTime The start time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] endTime   The end time in "dd/MM/YYYY hh:mm:ss" format.
     * @param[in] state     The state code or empty for all.
     * @param[in] priority  The priority or empty for all.
     * @param[in] category  The category or empty for all.
     * @param[in] desc      The space separated description keywords
     *                      (case-insensitive) or empty if not applicable.
     * @param[in] status    The status.
     * @param[in] resources The comma separated resource list or empty if not
     *                      applicable.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getIncidentHistory(const std::string &startTime,
                                const std::string &endTime,
                                const std::string &state,
                                const std::string &priority,
                                const std::string &category,
                                const std::string &desc,
                                const std::string &resources,
                                int                status);

    /**
     * Gets locations with a keyword.
     *
     * @param[in] key      The keyword.
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLocations(const std::string &key,
                          const std::string &username);

    /**
     * Gets locations with a keyword near a point.
     *
     * @param[in] key      The keyword.
     * @param[in] lat      The latitude.
     * @param[in] lon      The longitude.
     * @param[in] radius   The search radius in km.
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getLocations(const std::string &key,
                          const std::string &lat,
                          const std::string &lon,
                          const std::string &radius,
                          const std::string &username);

    /**
     * Gets terminal locations near a point.
     *
     * @param[in] lat    The latitude.
     * @param[in] lon    The longitude.
     * @param[in] radius The search radius in km.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getTerminals(const std::string &lat,
                          const std::string &lon,
                          const std::string &radius);

    /**
     * Gets the state and address at a point.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getAddress(const std::string &lat, const std::string &lon);

    /**
     * Gets PTT history entries of a call.
     *
     * @param[in] callKey The call key.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getPttHistory(const std::string &callKey);

    /**
     * Gets all Standard bookmarks.
     *
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getStdBookmarks();

    /**
     * Gets all User Defined bookmarks for a user.
     *
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getUsrBookmarks(const std::string &username);

    /**
     * Checks whether a username already exists.
     *
     * @param[in] type        The user type - eUserType.
     * @param[in] checkActive true to check user active status.
     * @param[in] username    The username.
     * @return 1 if user is active, 0 if inactive, or -1 if does not exist.
     */
    int userExists(int type, bool checkActive, const std::string &username);

    /**
     * Saves a user's data.
     *
     * @param[in] type     The user type - eUserType.
     * @param[in] active   true to check user active status.
     * @param[in] username The username.
     * @param[in] password The user password.
     * @param[in] group    The user group ID.
     * @param[in] creator  Current server username.
     * @param[in] fleet    Fleet ID. Only used if non-negative in STM-nwk.
     * @return true if successful.
     */
    bool saveUser(int                type,
                  bool               active,
                  const std::string &username,
                  const std::string &password,
                  const std::string &group,
                  const std::string &creator,
                  int                fleet = -1);

    /**
     * Deletes a user.
     *
     * @param[in] type     The user type - eUserType.
     * @param[in] username The username.
     * @return true if successful.
     */
    bool deleteUser(int type, const std::string &username);

    /**
     * Gets all users.
     *
     * @param[in] type The user type - eUserType.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getUsers(int type);

    /**
     * Increments user failed login count.
     *
     * @param[in] username The username.
     * @return The count or 0 on failure.
     */
    int incrFailedLoginCount(const std::string &username);

    /**
     * Activates or deactivates a user and resets failed login count.
     *
     * @param[in] username The username.
     * @param[in] isActive true to activate.
     * @return true if successful.
     */
    bool setUserActive(const std::string &username, bool isActive);

    /**
     * Deletes a Status Message.
     *
     * @param[in] code The Status Message code.
     * @return true if successful.
     */
    bool deleteStatusMsg(const std::string &code);

    /**
     * Saves a Status Message.
     *
     * @param[in] type    The type.
     * @param[in] code    The code.
     * @param[in] text    The text.
     * @param[in] creator Current server username.
     * @return true if successful.
     */
    bool saveStatusMsg(const std::string &type,
                       const std::string &code,
                       const std::string &text,
                       const std::string &creator);

    /**
     * Adds an audit trail record.
     *
     * @param[in] username The user name.
     * @param[in] actionId The action ID. See eAction.
     * @param[in] desc     The action description.
     * @return true if successful.
     */
    bool addAuditTrail(const std::string &username,
                       int                actionId,
                       const std::string &desc = "");

    /**
     * Gets all POI entries for a user.
     *
     * @param[in] username The username.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getPoi(const std::string &username);

    /**
     * Gets a POI.
     *
     * @param[in] id The POI ID.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getPoi(int id);

    /**
     * Gets the POI categories.
     *
     * @return The comma separated category list. Empty string on error.
     */
    std::string getPoiCategories();

    /**
     * Gets DB connection error message.
     *
     * @return The error message. Empty string if no error.
     */
    const std::string &getConnError(){ return mConnErrMsg; }

    /**
     * Checks whether a field value exists.
     *
     * @param[in] table The table name.
     * @param[in] field The field. See eField.
     * @param[in] value The field value.
     * @return true if exists.
     */
    bool hasValue(const std::string &table,
                  int                field,
                  const std::string &value);

    /**
     * Updates a field value in a record matching another (key) field value.
     *
     * @param[in] table    The table name.
     * @param[in] field    The field. See eField.
     * @param[in] newValue The new value.
     * @param[in] keyField The key field. See eField.
     * @param[in] keyValue The key value.
     * @return true if successful.
     */
    bool updateValue(const std::string &table,
                     int                field,
                     const std::string &newValue,
                     int                keyField,
                     const std::string &keyValue);

    /**
     * Updates a table record identified by a key field.
     *
     * @param[in] table    The table name.
     * @param[in] keyField The key field. See eField.
     * @param[in] keyValue The key value.
     * @param[in] record   The new record values.
     * @return true if successful.
     */
    bool updateRecord(const std::string &table,
                      int                keyField,
                      const std::string &keyValue,
                      RecordT           &record);

    /**
     * Updates a table record matching the key fields.
     *
     * @param[in] table  The table name.
     * @param[in] keys   The key fields and values.
     * @param[in] record The new record values.
     * @return true if successful.
     */
    bool updateRecord(const std::string &table,
                      RecordT           &keys,
                      RecordT           &record);

    /**
     * Inserts a record into database.
     *
     * @param[in]  table  The table name.
     * @param[in]  record The record.
     * @param[out] recId  The inserted record ID.
     * @return true if successful.
     */
    bool insertRecord(const std::string &table,
                      RecordT           &record,
                      int               *recId = 0);

    /**
     * Deletes all table records containing a field value.
     *
     * @param[in] table The table name.
     * @param[in] field The key field. See eField.
     * @param[in] value The key value.
     * @return true if successful.
     */
    bool deleteRecords(const std::string &table,
                       int                field,
                       const std::string &value);

    /**
     * Deletes a table record matching the key fields.
     *
     * @param[in] table The table name.
     * @param[in] keys  The key fields and values.
     * @return true if successful.
     */
    bool deleteRecord(const std::string &table,
                      RecordT           &keys);

    /**
     * Monitors the server connection status, and reconnects if disconnected.
     */
    void connectThread();

    /**
     * Gets a terminal's GPS information over a time period.
     *
     * @param[in] issi      The ISSI.
     * @param[in] startTime The start time in 'DD/MM/YYYY hh:mm' format.
     * @param[in] endTime   The end time.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getGps(const std::string &issi,
                    const std::string &startTime,
                    const std::string &endTime);

    /**
     * Gets GPS information over a time period, for either specific terminals or
     * all terminals of specific types.
     *
     * @param[in] issis     The space-separated ISSIs. Empty string to get by
     *                      type.
     * @param[in] types     The space-separated type values.
     *                      See SubsData::eTerminalType. Used only if issis is
     *                      empty.
     * @param[in] startTime The start time in 'DD/MM/YYYY hh:mm' format.
     * @param[in] endTime   The end time.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getGps(const std::string &issis,
                    const std::string &types,
                    const std::string &startTime,
                    const std::string &endTime);

    /**
     * Gets a navigation route from source location to destination.
     *
     * @param[in] srcLat The source location latitude.
     * @param[in] srcLon The source location longitude.
     * @param[in] dstLat The destination latitude.
     * @param[in] dstLon The destination longitude.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getRouting(const std::string &srcLat,
                        const std::string &srcLon,
                        const std::string &dstLat,
                        const std::string &dstLon);

    /**
     * Gets audit trail data over a time period.
     *
     * @param[in] id         The user ID or empty for all.
     * @param[in] action     The user action.
     * @param[in] cfgDetails The configuration details or empty for all.
     * @param[in] startTime  The start time in 'DD/MM/YYYY' format.
     * @param[in] endTime    The end time in 'DD/MM/YYYY' format.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getAuditTrail(const std::string &id,
                           const std::string &action,
                           const std::string &cfgDetails,
                           const std::string &startTime,
                           const std::string &endTime);

    /**
     * Sets the Logger object and database parameters.
     *
     * @param[in] logger   The Logger object.
     * @param[in] username The database username.
     * @param[in] pw       The database username's password.
     * @param[in] dbName   The database name.
     * @param[in] port     The database port.
     * @param[in] remoteIp The remote host IP. Required for remote clients.
     * @return true if successful.
     */
    static bool init(Logger            *logger,
                     const std::string &username,
                     const std::string &pw,
                     const std::string &dbName,
                     int                port,
                     const std::string &remoteIp = "");

    /**
     * Instantiates the singleton if it has not been created.
     *
     * @return The instance.
     */
    static DbInt &instance();

    /**
     * Deletes the single instance.
     */
    static void destroy();

    static const std::string &getConnStr() { return sConnStr; }

    /**
     * Checks whether a connection string contains the database server address.
     * If not, appends it.
     *
     * @param[in,out] str The connection string.
     * @param[in]     ip  The IP address.
     */
    static void chkConnStr(std::string &str, const std::string &ip);

    /**
     * Gets the action string of the specified action.
     *
     * @param[in] action The action. See eAction.
     * @return The action string.
     */
    static const std::string &getActionStr(int action);

    static const RecordT &getActions() { return sActionMap; }

private:
    typedef std::map<int, std::string> FieldNameMapT;

    bool                mStopped;
    DbConnT            *mConn;            //db connection
    std::string         mConnErrMsg;      //db connection error message
    PalThread::ThreadT  mConnectThreadId; //db connection status thread ID

    static bool            sIsCreated;
    static std::string     sConnStr;        //database connection string
    static DbInt          *sInstance;       //single class instance
    static Logger         *sLogger;         //logger object
    static PalLock::LockT  sSingletonLock;  //guards instance creation
    static FieldNameMapT   sFieldNameMap;   //field names
    static FieldNameMapT   sActionMap;      //actions

    /**
     * Constructor is private to prevent direct instantiation.
     */
    DbInt();

    ~DbInt();

    /**
     * Prevent copy construction.
     */
    DbInt(const DbInt &);

    /**
     * Prevent assignment operation.
     */
    DbInt &operator=(const DbInt &);

    /**
     * Checks current database connection status, and connects if necessary.
     *
     * @param[in] forceReconnect true to force reconnect if already connected.
     *                           Otherwise returns success in that case.
     * @param[in] chkOnly        true to only check status without actually
     *                           connecting. Ignored if forceReconnect is true.
     * @return true if successful.
     */
    bool connect(bool forceReconnect, bool chkOnly = false);

    /**
     * Gets all rows.
     *
     * @param[in] table The table name.
     * @return The result, or 0 on error. Caller takes ownership of the created
     *         object, and is responsible for deleting it.
     */
    QResult *getAll(const std::string &table);

    /**
     * Gets items from a table.
     *
     * @param[in] table The table name.
     * @param[in] field The item field.
     * @return The comma separated item list. Empty string on error.
     */
    std::string getTableData(const std::string &table, int field);

    /**
     * Creates a mapping of field values to string. Used to initialize the
     * static map member.
     *
     * @return The map.
     */
    static FieldNameMapT createFieldNameMap();

    /**
     * Creates a mapping of action values to string. Used to initialize the
     * static map member.
     *
     * @return The map.
     */
    static FieldNameMapT createActionMap();

    /**
     * Gets a field name.
     *
     * @param[in] id The field ID. See eField.
     * @return The field name if the ID exists, otherwise "X".
     */
    static const std::string &getFieldName(int id);
};
#endif //DBINT_H
