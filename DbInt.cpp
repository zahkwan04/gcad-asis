/**
 * Implementation of database operation functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: DbInt.cpp 1904 2025-02-19 06:59:58Z zulzaidi $
 * @author Muhd Hashim Wahab
 */
#include <assert.h>

#include "Locker.h"
#if defined(SNMP) && defined(SERVERAPP)
#include "SnmpAgent.h"
#endif
#include "Utils.h"
#include "DbInt.h"

//SQL to convert client time to database accepted format
#define SQLTOTIME(value) \
    string("TO_TIMESTAMP('" + value + "','DD/MM/YYYY HH24:MI')")

using namespace std;

//SQL tags for escaping single quotes or backslashes
static const string ESC1("$e1$");
static const string ESC2("$e2$");

bool                  DbInt::sIsCreated(false);
string                DbInt::sConnStr;
DbInt                *DbInt::sInstance(0);
Logger               *DbInt::sLogger(0);
DbInt::FieldNameMapT  DbInt::sFieldNameMap(createFieldNameMap());
DbInt::FieldNameMapT  DbInt::sActionMap(createActionMap());

#ifdef _WIN32
PalLock::LockT DbInt::sSingletonLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT DbInt::sSingletonLock(QMutex::Recursive);
#else
PalLock::LockT DbInt::sSingletonLock = PTHREAD_MUTEX_INITIALIZER;
#endif

const string DbInt::DB_TABLE_D_BOOKMARK        ("d_bookmark");
const string DbInt::DB_TABLE_D_CALL_HIST       ("d_call_hist");
const string DbInt::DB_TABLE_D_CALL_AUDIO      ("d_call_record");
const string DbInt::DB_TABLE_D_CONTACTS        ("d_contacts");
const string DbInt::DB_TABLE_D_COUNTRY_STATE   ("d_country_state");
const string DbInt::DB_TABLE_D_GPS             ("d_gps");
const string DbInt::DB_TABLE_D_INC_CATEGORY    ("d_incident_category");
const string DbInt::DB_TABLE_D_INC_HIST        ("d_incident_hist");
const string DbInt::DB_TABLE_D_INC_RSC         ("d_incident_resources");
const string DbInt::DB_TABLE_D_MMS_HIST        ("d_mms_hist");
const string DbInt::DB_TABLE_D_PHONEBOOK       ("d_phonebook");
const string DbInt::DB_TABLE_D_POI             ("d_poi");
const string DbInt::DB_TABLE_D_POI_CATEGORY    ("d_poi_category");
const string DbInt::DB_TABLE_D_PTT_HIST        ("d_ptt_hist");
const string DbInt::DB_TABLE_D_SDS_HIST        ("d_sds_hist");
const string DbInt::DB_TABLE_D_STSMSG          ("d_stsmsg");
const string DbInt::DB_TABLE_D_STSMSG_HIST     ("d_stsmsg_hist");
const string DbInt::DB_TABLE_D_STSMSG_TYPE     ("d_stsmsg_type");
const string DbInt::DB_TABLE_D_USER_CLIENT     ("d_user_client");
const string DbInt::DB_TABLE_D_USER_CLIENT_DISP("d_user_client_disp");
const string DbInt::DB_TABLE_D_USER_GROUP      ("d_user_group");
const string DbInt::DB_TABLE_D_USER_SERVER     ("d_user_server");

#ifndef NO_DB
static void *startConnectThread(void *arg)
{
    static_cast<DbInt *>(arg)->connectThread();
    return 0;
}
#endif

bool DbInt::isValid()
{
#ifdef NO_DB
    return true;
#else
    return connect(false);
#endif
}

DbInt::QResult *DbInt::queryExec(const string &query)
{
#ifdef NO_DB
    return 0;
#else
    if (!isValid())
        return 0;
    int retry = 1;
    do
    {
        PalLock::take(&sSingletonLock);
        QueryResultT *res = PQexec(mConn, query.c_str());
        PalLock::release(&sSingletonLock);
        ExecStatusType stat = PQresultStatus(res);
        if (stat == PGRES_COMMAND_OK || stat == PGRES_TUPLES_OK)
            return new QResult(res);
        LOGGER_ERROR(sLogger,
                     "DbInt::queryExec(string): Query failed, retry="
                     << retry << ".\n\"" << query << "\"\n"
                     << PQerrorMessage(mConn));
        PQclear(res);
        //reconnect on fatal error and retry on any error
        if (stat == PGRES_FATAL_ERROR && !connect(true))
            break;
    }
    while (retry-- != 0);
    return 0;
#endif //NO_DB
}

DbInt::QResult *DbInt::queryExec(const string       &query,
                                 vector<string>     &paramValues,
                                 const unsigned int *paramTypes)
{
#ifdef NO_DB
    return 0;
#else
    if (!isValid())
        return 0;
    const char **params = new const char *[paramValues.size()];
    const char **p = params;
    for (const auto &s : paramValues)
    {
        *p++ = s.c_str();
    }
    int retry = 1;
    do
    {
        PalLock::take(&sSingletonLock);
        QueryResultT *res = PQexecParams(mConn, query.c_str(),
                                         paramValues.size(), paramTypes,
                                         params, NULL, NULL, 1);
        PalLock::release(&sSingletonLock);
        ExecStatusType stat = PQresultStatus(res);
        if (stat == PGRES_COMMAND_OK || stat == PGRES_TUPLES_OK)
        {
            delete [] params;
            return new QResult(res);
        }
        LOGGER_ERROR(sLogger,
                     "DbInt::queryExec(params): Query failed, retry="
                     << retry << ".\n\""<< query << "\"\n"
                     << PQerrorMessage(mConn));
        PQclear(res);
        //reconnect on fatal error and retry on any error
        if (stat == PGRES_FATAL_ERROR && !connect(true))
            break;
    }
    while (retry-- != 0);
    delete [] params;
    return 0;
#endif //NO_DB
}

bool DbInt::commandExec(const string &cmd)
{
    QResult *res = queryExec(cmd);
    delete res;
    return (res != 0);
}

string DbInt::getStr(const string &table,
                     int           field,
                     int           keyField,
                     const string &keyValue)
{
    string str;
    QResult *res = getRows(table, keyField, keyValue);
    if (res != 0)
    {
        if (res->getNumRows() == 1)
            res->getFieldValue(field, str);
        delete res;
    }
    return str;
}

int DbInt::getValue(const string &table,
                    int           field,
                    int           keyField,
                    const string &keyValue)
{
    int val = -1;
    QResult *res = getRows(table, keyField, keyValue);
    if (res != 0)
    {
        if (res->getNumRows() == 1)
            res->getFieldValue(field, val);
        delete res;
    }
    return val;
}

DbInt::QResult *DbInt::getRows(const string &table,
                               int           field,
                               const string &keyValue)
{
    return queryExec("SELECT * FROM " + table + " WHERE " +
                     getFieldName(field) + "=" + ESC1 + keyValue + ESC1);
}

DbInt::QResult *DbInt::getBranches()
{
    return queryExec("SELECT * FROM fn_get_branches()");
}

bool DbInt::saveBranch(int branch, int cluster, const string &name)
{
    return commandExec("SELECT * FROM fn_save_branch(" +
                       Utils::toString(branch) + "," +
                       Utils::toString(cluster) + "," +
                       ESC1 + name + ESC1 + ")");
}

bool DbInt::deleteBranch(int branch)
{
    return commandExec("SELECT * FROM fn_delete_branch(" +
                       Utils::toString(branch) + ")");
}

bool DbInt::saveBranchIds(int branch, int field, const string &ids)
{
    return commandExec("SELECT * FROM fn_save_branch_ids(" +
                       Utils::toString(branch) + ",'" + getFieldName(field) +
                       "','" + ids + "')");
}

DbInt::QResult *DbInt::getSubsData(bool ind)
{
    if (ind)
        return queryExec("SELECT * FROM fn_get_subsdata()");
    return queryExec("SELECT * FROM fn_get_grpdata()");
}

bool DbInt::saveSubsData(bool          ind,
                         const string &ssi,
                         const string &type,
                         const string &desc)
{
    if (ind)
        return commandExec("SELECT * FROM fn_save_subsdata(" + ssi + "," +
                           type + "," + ESC1 + desc + ESC1 + ")");
    return commandExec("SELECT * FROM fn_save_grpdata(" + ssi + "," + ESC1 +
                       desc + ESC1 + ")");
}

bool DbInt::deleteSubsData(bool ind, const string &ssi)
{
    if (ind)
        return commandExec("SELECT * FROM fn_delete_subsdata(" + ssi + ")");
    return commandExec("SELECT * FROM fn_delete_grpdata(" + ssi + ")");
}

bool DbInt::getTerminalTypes(Int2StringMapT &tTypes)
{
    QResult *res = queryExec("SELECT * FROM fn_get_terminal_types()");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger,
                     "DbInt::getTerminalTypes: DB Data query failed.");
        return false;
    }
    int type;
    string desc;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        if (res->getFieldValue(FIELD_ITEMCAT, type, i) &&
            res->getFieldValue(FIELD_DESC, desc, i))
        {
            Utils::trim(desc);
            tTypes[type] = desc;
        }
    }
    delete res;
    return true;
}

bool DbInt::getStatusTypes(Int2StringMapT &smTypes)
{
    QResult *res = queryExec("SELECT * FROM fn_get_sts_types()");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getStatusTypes: DB Data query failed.");
        return false;
    }
    int type;
    string desc;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        if (res->getFieldValue(FIELD_TYPE, type, i) &&
            res->getFieldValue(FIELD_TYPE_DESC, desc, i))
        {
            Utils::trim(desc);
            smTypes[type] = desc;
        }
    }
    delete res;
    return true;
}

bool DbInt::getStatusCodes(vector<StatusData> &scList, int fleet)
{
    QResult *res = queryExec("SELECT * FROM fn_get_sts_codes(" +
                             ((fleet >= 0)? Utils::toString(fleet): "") + ")");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getStatusCodes: DB Data query failed.");
        return false;
    }
    StatusData sd;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        res->getFieldValue(FIELD_STATUSCODE_CODE, sd.code, i);
        res->getFieldValue(FIELD_STATUSCODE_TYPE, sd.type, i);
        res->getFieldValue(FIELD_STATUSCODE_TEXT, sd.text, i);
        Utils::trim(sd.text);
        scList.push_back(sd);
    }
    delete res;
    return true;
}

bool DbInt::getCountryStates(DataMapT &states)
{
    QResult *res = queryExec("SELECT * FROM fn_get_country_states()");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getCountryStates: DB Data query "
                     "failed.");
        return false;
    }
    int i = res->getNumRows() - 1;
    if (i < 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getCountryStates: Empty data.");
        i = 0;
    }
    else
    {
        string code;
        string name;
        for (; i>=0; --i)
        {
            res->getFieldValue(FIELD_STATE_DESC, name, i);
            if (Utils::trim(name) != 0)
            {
                res->getFieldValue(FIELD_STATE_CODE, code, i);
                states[name] = code;
            }
        }
    }
    delete res;
    return (i != 0);
}

DbInt::QResult *DbInt::getContacts(const string &username)
{
    QResult *res = queryExec("SELECT * FROM fn_get_contacts('" + username +
                             "')");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getContacts: DB query failed for "
                     << username);
    }
    return res;
}

DbInt::QResult *DbInt::getPhonebook(const string &username)
{
    QResult *res = queryExec("SELECT * FROM fn_get_phonebook('" + username +
                             "')");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getPhonebook: DB query failed for "
                     << username);
    }
    return res;
}

bool DbInt::getIncidentCategories(DataMapT &categories)
{
    QResult *res = queryExec("SELECT * FROM fn_get_incident_cat()");
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getIncidentCategories: DB Data query "
                     "failed.");
        return false;
    }
    int i = res->getNumRows() - 1;
    if (i < 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getIncidentCategories: Empty data.");
        i = 0;
    }
    else
    {
        string cat;
        string catBase;
        for (; i>=0; --i)
        {
            res->getFieldValue(FIELD_DESC, cat, i);
            if (Utils::trim(cat) != 0)
            {
                res->getFieldValue(FIELD_BASE, catBase, i);
                categories[cat] = catBase;
            }
        }
    }
    delete res;
    return (i != 0);
}

DbInt::QResult *DbInt::getIncidentsByExclStatus(int exclStatus)
{
    return queryExec("SELECT * FROM fn_get_incidents_by_excl_status(" +
                     Utils::toString(exclStatus) + ")");
}

int DbInt::lockIncident(int id, int holder)
{
    QResult *res = queryExec("SELECT * FROM fn_lock_incident_hist(" +
                             Utils::toString(id) + "," +
                             Utils::toString(holder) + ")");
    if (res == 0)
        return -1;
    holder = res->getReturnInt();
    delete res;
    return holder;
}

int DbInt::getLockedIncident(const string &username)
{
    QResult *res = queryExec("SELECT * FROM fn_get_locked_incident_hist(" +
                             username + ")");
    if (res == 0)
        return -1;
    int id = res->getReturnInt();
    delete res;
    return id;
}

int DbInt::createIncident(const RecordT     &record,
                          const set<string> &resources)
{
    if (record.empty())
        return -1;
    string query("SELECT * FROM fn_create_incident('(");
    string values(ESC2);
    values.append("(");
    RecordT::const_iterator it = record.begin();
    for (; it!=record.end(); ++it)
    {
        if (it != record.begin())
        {
            query.append(",");
            values.append(",");
        }
        switch (it->first)
        {
            case FIELD_CREATED_DATE:
            case FIELD_INC_CLOSED_DATE:
            case FIELD_INC_DISP_DATE:
            case FIELD_INC_ONSCENE_DATE:
            case FIELD_INC_RECVD_DATE:
            case FIELD_MODIFIED_DATE:
                //client uses format 'DD/MM/YYYY HH24:MI:SS' - convert to
                //database format
                if (it->second.find("/") != string::npos)
                {
                    values.append(SQLTOTIME(it->second));
                    break;
                }
                //fallthrough
            default:
                values.append(ESC1).append(it->second).append(ESC1);
                break;
        }
        query.append(getFieldName(it->first));
    }
    query.append(")',").append(values).append(")").append(ESC2).append(",'");
    if (!resources.empty())
        query.append(Utils::toString(resources, ','));
    query.append("')");
    QResult *res = queryExec(query);
    if (res == 0)
        return -1;
    int id = res->getReturnInt();
    delete res;
    return id;
}

bool DbInt::updateIncident(const string      &id,
                           const RecordT     &record,
                           const set<string> &resources)
{
    if (record.empty())
        return false;
    string query("SELECT * FROM fn_update_incident('");
    query.append(id).append("',").append(ESC2);
    RecordT::const_iterator it = record.begin();
    for (; it!=record.end(); ++it)
    {
        if (it != record.begin())
             query.append(",");
        query.append(getFieldName(it->first)).append("=");
        switch (it->first)
        {
            case FIELD_CREATED_DATE:
            case FIELD_INC_CLOSED_DATE:
            case FIELD_INC_DISP_DATE:
            case FIELD_INC_ONSCENE_DATE:
            case FIELD_INC_RECVD_DATE:
            case FIELD_MODIFIED_DATE:
                if (it->second.find("/") != string::npos)
                {
                    query.append(SQLTOTIME(it->second));
                    break;
                }
                if (it->second.empty())
                {
                    query.append("NULL");
                    break;
                }
                //fallthrough
            default:
                query.append(ESC1).append(it->second).append(ESC1);
                break;
        }
    }
    query.append(ESC2).append(",'");
    if (!resources.empty())
        query.append(Utils::toString(resources, ','));
    query.append("')");
    QResult *res = queryExec(query);
    delete res;
    return (res != 0);
}

DbInt::QResult *DbInt::getCallHistory(const string &startTime,
                                      const string &endTime,
                                      const string &type,
                                      int           from,
                                      int           to,
                                      bool          doAnd)
{
    return queryExec("SELECT * FROM fn_get_call_hist('" + startTime + "','" +
                     endTime + "','" + type + "'," + Utils::toString(from) +
                     "," + Utils::toString(to) + "," +
                     ((doAnd)? "true": "false") + ")");
}

DbInt::QResult *DbInt::getLastCall(int from, int to)
{
    return queryExec("SELECT * FROM fn_get_last_call(" +
                     Utils::toString(from) + "," + Utils::toString(to) + ")");
}

DbInt::QResult *DbInt::getSdsHistory(const string &startTime,
                                     const string &endTime,
                                     const string &msg,
                                     int           from,
                                     int           to,
                                     bool          doAnd)
{
    return queryExec("SELECT * FROM fn_get_sds_hist('" + startTime + "','" +
                     endTime + "'," + ESC1 + msg + ESC1 + "," +
                     Utils::toString(from) + "," + Utils::toString(to) + "," +
                     ((doAnd)? "true": "false") + ")");
}

DbInt::QResult *DbInt::getLastSds(int from, int to)
{
    return queryExec("SELECT * FROM fn_get_last_sds(" +
                     Utils::toString(from) + "," + Utils::toString(to) + ")");
}

DbInt::QResult *DbInt::getStsMsgHistory(const string &startTime,
                                        const string &endTime,
                                        const string &text,
                                        int           from,
                                        int           to,
                                        bool          doAnd)
{
    return queryExec("SELECT * FROM fn_get_stsmsg_hist('" + startTime +
                     "','" + endTime + "','" + text + "'," +
                     Utils::toString(from) + "," + Utils::toString(to) + "," +
                     ((doAnd)? "true": "false") + ")");
}

DbInt::QResult *DbInt::getLastSts(int from, int to)
{
    return queryExec("SELECT * FROM fn_get_last_stsmsg(" +
                     Utils::toString(from) + "," + Utils::toString(to) + ")");
}

DbInt::QResult *DbInt::getMmsHistory(const std::string &startTime,
                                     const std::string &endTime,
                                     const std::string &msg,
                                     int                from,
                                     int                to,
                                     bool               doAnd)
{
    return queryExec("SELECT * FROM fn_get_mms_hist('" + startTime + "','" +
                     endTime + "'," + ESC1 + msg + ESC1 + "," +
                     Utils::toString(from) + "," + Utils::toString(to) + "," +
                     ((doAnd)? "true": "false") + ")");
}

DbInt::QResult *DbInt::getLastMms(int from, int to)
{
    return queryExec("SELECT * FROM fn_get_last_mms(" +
                     Utils::toString(from) + "," + Utils::toString(to) + ")");
}

DbInt::QResult *DbInt::getMsgHistory(const string &startTime,
                                     const string &endTime,
                                     int           from,
                                     int           to,
                                     bool          doAnd)
{
    return queryExec("SELECT * FROM fn_get_msg_hist('" + startTime + "','" +
                     endTime + "'," + Utils::toString(from) + "," +
                     Utils::toString(to) + "," + ((doAnd)? "true": "false") +
                     ")");
}

DbInt::QResult *DbInt::getIncidentHistory(int id)
{
    return queryExec("SELECT * FROM fn_get_incident_hist(" +
                     Utils::toString(id) + ")");
}

DbInt::QResult *DbInt::getIncidentHistory(const string &startTime,
                                          const string &endTime,
                                          const string &state,
                                          const string &priority,
                                          const string &category,
                                          const string &desc,
                                          const string &resources,
                                          int           status)
{
    return queryExec("SELECT * FROM fn_get_incident_hist('" + startTime +
                     "','" + endTime + "','" + state + "','" + priority +
                     "','" + category + "'," + ESC1 + desc + ESC1 + ",'" +
                     Utils::toString(status) + "'," + "'" + resources + "')");
}

DbInt::QResult *DbInt::getLocations(const string &key, const string &username)
{
    return queryExec("SELECT * FROM fng_get_location(" + ESC1 + key + ESC1 +
                     ",'" + username + "')");
}

DbInt::QResult *DbInt::getLocations(const string &key,
                                    const string &lat,
                                    const string &lon,
                                    const string &radius,
                                    const string &username)
{
    return queryExec("SELECT * FROM fng_get_location(" + ESC1 + key + ESC1 +
                     ",'" + lon + "','" + lat + "','" + username + "','" +
                     radius + "')");
}

DbInt::QResult *DbInt::getTerminals(const string &lat,
                                    const string &lon,
                                    const string &radius)
{
    return queryExec("SELECT * FROM fng_get_terminals('" + lon + "','" +
                     lat + "','" + radius + "')");
}

DbInt::QResult *DbInt::getAddress(const string &lat, const string &lon)
{
    return queryExec("SELECT * FROM fng_get_address('" + lon + "','" + lat +
                     "')");
}

DbInt::QResult *DbInt::getPttHistory(const string &callKey)
{
    return queryExec("SELECT * FROM fn_get_ptt_hist(" + callKey + ")");
}

DbInt::QResult *DbInt::getStdBookmarks()
{
    return queryExec("SELECT * FROM fn_get_std_bookmarks()");
}

DbInt::QResult *DbInt::getUsrBookmarks(const string &username)
{
    return queryExec("SELECT * FROM fn_get_usr_bookmarks('" + username +
                     "')");
}

int DbInt::userExists(int type, bool checkActive, const string &username)
{
    QResult *res = queryExec("SELECT * FROM fn_user_exists(" +
                             Utils::toString(type) + ",'" +
                             Utils::toString(checkActive) + "','" +
                             username + "')");
    if (res == 0)
        return -1;
    int retVal = res->getReturnInt();
    delete res;
    return retVal;
}

bool DbInt::saveUser(int           type,
                     bool          active,
                     const string &username,
                     const string &password,
                     const string &group,
                     const string &creator,
                     int           fleet)
{
    return commandExec("SELECT * FROM fn_save_user(" + Utils::toString(type) +
                       ",'" + username + "','" + password + "'," +
                       ((fleet >= 0)? Utils::toString(fleet) + ",": "") + "'" +
                       group + "','" + Utils::toString(active) + "','" +
                       creator + "')");
}

bool DbInt::deleteUser(int type, const string &username)
{
    return commandExec("SELECT * FROM fn_delete_user(" +
                       Utils::toString(type) + ",'" + username + "')");
}

DbInt::QResult *DbInt::getUsers(int type)
{
    return queryExec("SELECT * FROM fn_get_users(" + Utils::toString(type) +
                     ")");
}

int DbInt::incrFailedLoginCount(const string &username)
{
    QResult *res = queryExec("SELECT * FROM fn_incr_failed_login_count('" +
                             username + "')");
    if (res == 0)
        return 0;
    int retVal = res->getReturnInt();
    delete res;
    return retVal;
}

bool DbInt::setUserActive(const string &username, bool isActive)
{
    RecordT data;
    data[FIELD_USER_ACTIVE] = (isActive)? "true": "false";
    data[FIELD_USER_FAILED_COUNT] = "0";
    return updateRecord(DB_TABLE_D_USER_CLIENT, FIELD_USER_NAME, username, data);
}

bool DbInt::deleteStatusMsg(const string &code)
{
    return commandExec("SELECT * FROM fn_delete_stsmsg(" + code + ")");
}

bool DbInt::saveStatusMsg(const string &type,
                          const string &code,
                          const string &text,
                          const string &creator)
{
    return commandExec("SELECT * FROM fn_save_stsmsg(" + type + "," + code +
                       "," + ESC1 + text + ESC1 + ",'" + creator + "')");
}

bool DbInt::addAuditTrail(const string &username,
                          int           actionId,
                          const string &desc)
{
    if (sActionMap.count(actionId) == 0)
        return false;
    return commandExec("SELECT * FROM fn_add_audit_trail('" + username +
                       "','" + sActionMap[actionId] + "'," + ESC1 + desc +
                       ESC1 + ")");
}

DbInt::QResult *DbInt::getPoi(const string &username)
{
    return queryExec("SELECT * FROM fn_get_poi('" + username + "')");
}

DbInt::QResult *DbInt::getPoi(int id)
{
    return queryExec("SELECT * FROM fn_get_poi(" + Utils::toString(id) + ")");
}

string DbInt::getPoiCategories()
{
    return getTableData(DB_TABLE_D_POI_CATEGORY, FIELD_CATEGORY);
}

bool DbInt::hasValue(const string &table, int field, const string &keyValue)
{
    bool retVal = false;
    QResult *res = getRows(table, field, keyValue);
    if (res != 0)
    {
        retVal = (res->getNumRows() > 0);
        delete res;
    }
    return retVal;
}

bool DbInt::updateValue(const string &table,
                        int           field,
                        const string &newValue,
                        int           keyField,
                        const string &keyValue)
{
    return commandExec("UPDATE " + table + " SET " + getFieldName(field) +
                       "=" + ESC1 + newValue + ESC1 + " WHERE " +
                       getFieldName(keyField) + "=" + ESC1 + keyValue + ESC1);
}

bool DbInt::updateRecord(const string &table,
                         int           keyField,
                         const string &keyValue,
                         RecordT      &record)
{
    if (table.empty() || record.empty())
        return false;

    string query("UPDATE " + table + " SET ");
    RecordT::iterator it = record.begin();
    for (; it!=record.end(); ++it)
    {
        if (it != record.begin())
             query.append(",");
        query.append(getFieldName(it->first)).append("=");
        switch (it->first)
        {
            case FIELD_SHAPE:
                query.append("'POINT(").append(it->second).append(")'");
                break;
            case FIELD_CREATED_DATE:
            case FIELD_MODIFIED_DATE:
            case FIELD_TIME:
                //client uses format 'DD/MM/YYYY HH24:MI:SS' - convert to
                //database format
                if (it->second.find("/") != string::npos)
                {
                    query.append(SQLTOTIME(it->second));
                    break;
                }
                if (it->second.empty())
                {
                    query.append("NULL");
                    break;
                }
                //fallthrough
            default:
                query.append(ESC1).append(it->second).append(ESC1);
                break;
        }
    }
    query.append(" WHERE ").append(getFieldName(keyField)).append("=")
         .append(ESC1).append(keyValue).append(ESC1);
    return commandExec(query);
}

bool DbInt::updateRecord(const string &table,
                         RecordT      &keys,
                         RecordT      &record)
{
    if (table.empty() || keys.empty() || record.empty())
        return false;
    string query("UPDATE ");
    RecordT::iterator it = record.begin();
    query.append(table).append(" SET ")
         .append(getFieldName(it->first)).append("=")
         .append(ESC1).append(it->second).append(ESC1);
    for (++it; it!=record.end(); ++it)
    {
        query.append(",").append(getFieldName(it->first)).append("=")
             .append(ESC1).append(it->second).append(ESC1);
    }
    it = keys.begin();
    query.append(" WHERE ").append(getFieldName(it->first)).append("=")
         .append(ESC1).append(it->second).append(ESC1);
    for (++it; it!=keys.end(); ++it)
    {
        query.append(" AND ").append(getFieldName(it->first)).append("=")
             .append(ESC1).append(it->second).append(ESC1);
    }
    return commandExec(query);
}

bool DbInt::insertRecord(const string &table, RecordT &record, int *recId)
{
    if (record.empty())
        return false;

    string query("INSERT INTO " + table);
    string values;
    query.append(" (");
    RecordT::iterator it = record.begin();
    for (; it!=record.end(); ++it)
    {
        if (it != record.begin())
        {
            query.append(",");
            values.append(",");
        }
        switch (it->first)
        {
            case FIELD_SHAPE:
                values.append("'POINT(").append(it->second).append(")'");
                break;
            case FIELD_CREATED_DATE:
            case FIELD_MODIFIED_DATE:
            case FIELD_TIME:
                if (it->second.find("/") != string::npos)
                {
                    values.append(SQLTOTIME(it->second));
                    break;
                }
                //fallthrough
            default:
                values.append(ESC1).append(it->second).append(ESC1);
                break;
        }
        query.append(getFieldName(it->first));
    }
    query.append(") VALUES (").append(values).append(")");
    if (recId == 0)
    {
        query.append(" ON CONFLICT DO NOTHING");
        return commandExec(query);
    }
    query.append(" RETURNING " + getFieldName(FIELD_ID));
    QResult *res = queryExec(query);
    if (res == 0)
        return false;
    bool retVal = (res->getNumRows() != 0 &&
                   res->getFieldValue(FIELD_ID, *recId, 0));
    delete res;
    return retVal;
}

bool DbInt::deleteRecords(const string &table, int field, const string &value)
{
    return commandExec("DELETE FROM " + table + " WHERE " +
                       getFieldName(field) + "=" + ESC1 + value + ESC1);
}

bool DbInt::deleteRecord(const string &table, RecordT &keys)
{
    if (table.empty() || keys.empty())
        return false;
    string query("DELETE FROM ");
    RecordT::iterator it = keys.begin();
    query.append(table).append(" WHERE ")
         .append(getFieldName(it->first)).append("=")
         .append(ESC1).append(it->second).append(ESC1);
    for (++it; it!=keys.end(); ++it)
    {
        query.append(" AND ").append(getFieldName(it->first)).append("=")
             .append(ESC1).append(it->second).append(ESC1);
    }
    return commandExec(query);
}

void DbInt::connectThread()
{
    LOGGER_DEBUG(sLogger, "DbInt::connectThread started.");
#ifndef NO_DB
#if defined(SNMP) && defined(SERVERAPP)
    bool alertSent = false;
#endif
    int i;
    while (!mStopped)
    {
        if (!connect(false))
        {
#if defined(SNMP) && defined(SERVERAPP)
            if (!alertSent)
                alertSent = SNMP_TRAP(SnmpAgent::TRAP_DB_STAT,
                                      SnmpAgent::VAL_DISC);
#endif
            if (!mStopped)
                PalThread::sleep(5);
            continue;
        }
#if defined(SNMP) && defined(SERVERAPP)
        if (alertSent)
            alertSent = !SNMP_TRAP(SnmpAgent::TRAP_DB_STAT, SnmpAgent::VAL_CONN);
#endif
        //30 seconds sleep - quantize to 5 seconds to allow faster exit
        for (i=6; i>0 && !mStopped; --i)
        {
            PalThread::sleep(5);
        }
    }
#endif //!NO_DB
    LOGGER_DEBUG(sLogger, "DbInt::connectThread exiting.");
}

DbInt::QResult *DbInt::getGps(const string &issi,
                              const string &startTime,
                              const string &endTime)
{
    return queryExec("SELECT * FROM fn_get_gps(" + issi + ",'" + startTime +
                     "','" + endTime + "')");
}

DbInt::QResult *DbInt::getGps(const string &issis,
                              const string &types,
                              const string &startTime,
                              const string &endTime)
{
    return queryExec("SELECT * FROM fn_get_gps_hist('" + issis + "','" + types +
                     "','" + startTime + "','" + endTime + "')");
}

DbInt::QResult *DbInt::getRouting(const string &srcLat,
                                  const string &srcLon,
                                  const string &dstLat,
                                  const string &dstLon)
{
    return queryExec("SELECT * FROM fng_get_routing(" + srcLat + "," +
                     srcLon + "," + dstLat + "," + dstLon + ")");
}

DbInt::QResult *DbInt::getAuditTrail(const string &id,
                                     const string &action,
                                     const string &cfgDetails,
                                     const string &startTime,
                                     const string &endTime)
{
    return queryExec("SELECT * FROM fn_get_audit_trail('" + id + "','" +
                     action + "'," + ESC1 + cfgDetails + ESC1 + ",'" +
                     startTime + "','" + endTime + "')");
}

bool DbInt::init(Logger       *logger,
                 const string &username,
                 const string &pw,
                 const string &dbName,
                 int           port,
                 const string &remoteIp)
{
    if (logger == 0 || username.empty() || pw.empty() || dbName.empty() ||
        port <= 0)
    {
        assert("Bad param in DbInt::init" == 0);
        return false;
    }
    sLogger = logger;
    string newConnStr("user=");
    newConnStr.append(username).append(" password=").append(pw)
              .append(" port=").append(Utils::toString(port))
              .append(" dbname=").append(dbName);
    if (!remoteIp.empty())
        newConnStr.append(" hostaddr=").append(remoteIp);
    if (newConnStr == sConnStr)
        return instance().isValid();
    sConnStr = newConnStr;
    return instance().connect(true);
}

DbInt &DbInt::instance()
{
    //a modified Double Checked Locking Pattern for thread-safe Singleton,
    //lock obtained only before/during creation
    if (!sIsCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!sIsCreated) //thread-safety double check
        {
            sInstance = new DbInt();
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                sIsCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

void DbInt::destroy()
{
    PalLock::take(&sSingletonLock);
    sIsCreated = false;
    delete sInstance;
    sInstance = 0;
    sConnStr.clear();
    PalLock::release(&sSingletonLock);
}

void DbInt::chkConnStr(string &str, const string &ip)
{
    if (str.find(" hostaddr=") == string::npos)
        str.append(" hostaddr=").append(ip);
}

const string &DbInt::getActionStr(int action)
{
    assert(sActionMap.count(action) != 0);
    return sActionMap[action];
}

DbInt::DbInt() : mStopped(false), mConn(0), mConnectThreadId(0)
{
    assert(sLogger != 0);
}

DbInt::~DbInt()
{
    if (mConnectThreadId != 0)
    {
        mStopped = true;
        PalThread::stop(mConnectThreadId);
    }
#ifndef NO_DB
    if (mConn != 0)
        PQfinish(mConn);
#endif
}

bool DbInt::connect(bool forceReconnect)
{
#ifdef NO_DB
    return false;
#else
    Locker lock(&sSingletonLock);
    if (mConn != 0)
    {
        if (!forceReconnect && PQstatus(mConn) == CONNECTION_OK)
            return true;
        PQfinish(mConn);
    }
    LOGGER_VERBOSE(sLogger, "Connecting to database...");
    mConn = PQconnectdb(sConnStr.c_str());
    if (mConn != 0)
    {
        mConnErrMsg = PQerrorMessage(mConn);
        if (PQstatus(mConn) == CONNECTION_OK)
        {
            LOGGER_INFO(sLogger, "Database connected");
        }
        else
        {
            PQfinish(mConn);
            mConn = 0;
        }
    }
    if (mConn == 0)
        LOGGER_ERROR(sLogger, "DbInt::connect: DB connection failed: "
                     "\nPQerrorMessage " << mConnErrMsg);
    if (mConnectThreadId == 0)
        PalThread::start(&mConnectThreadId, startConnectThread, this);
    return (mConn != 0);
#endif //NO_DB
}

DbInt::QResult *DbInt::getAll(const string &table)
{
    return queryExec("SELECT * FROM " + table + " ORDER BY 1 DESC");
}

string DbInt::getTableData(const string &table, int field)
{
    QResult *res = getAll(table);
    if (res == 0)
    {
        LOGGER_ERROR(sLogger, "DbInt::getTableData: DB query failed for "
                     << table << " field " << getFieldName(field));
        return "";
    }
    string value;
    string s;
    int i = res->getNumRows() - 1;
    if (i >= 0)
    {
        res->getFieldValue(field, value, i);
        s = value;
        for (--i; i>=0; --i)
        {
            res->getFieldValue(field, value, i);
            s.append(",").append(value);
        }
    }
    delete res;
    return s;
}

DbInt::FieldNameMapT DbInt::createFieldNameMap()
{
    FieldNameMapT m;
    m[DbInt::FIELD_UNDEFINED]           = "X";
    m[DbInt::FIELD_ADDRESS]             = "address";
    m[DbInt::FIELD_AUDIO_PATH]          = "record_path";
    m[DbInt::FIELD_AUDIT_ACTION]        = "audit_action";
    m[DbInt::FIELD_AUDIT_DATE]          = "audit_date";
    m[DbInt::FIELD_AUDIT_DESC]          = "audit_desc";
    m[DbInt::FIELD_BASE]                = "base";
    m[DbInt::FIELD_BR_BRANCH]           = "branch";
    m[DbInt::FIELD_BR_CLUSTER]          = "cluster";
    m[DbInt::FIELD_BR_DISP]             = "disp";
    m[DbInt::FIELD_BR_EM_SSI]           = "em_ssi";
    m[DbInt::FIELD_BR_MOBILE]           = "mobile";
    m[DbInt::FIELD_BR_SVR]              = "server";
    m[DbInt::FIELD_CALL_DURATION]       = "duration";
    m[DbInt::FIELD_CALL_HOOK_METHOD]    = "hook_method";
    m[DbInt::FIELD_CALL_ID]             = "call_id";
    m[DbInt::FIELD_CALL_SIMPLEX_DUPLEX] = "simplex_duplex";
    m[DbInt::FIELD_CALL_TYPE]           = "type";
    m[DbInt::FIELD_CATEGORY]            = "category";
    m[DbInt::FIELD_COORDS]              = "coords";
    m[DbInt::FIELD_CREATED_BY]          = "created_by";
    m[DbInt::FIELD_CREATED_DATE]        = "created_date";
    m[DbInt::FIELD_DATETIME]            = "datetime";
    m[DbInt::FIELD_DESC]                = "descr";
    m[DbInt::FIELD_DISC_CAUSE]          = "disc_cause";
    m[DbInt::FIELD_DISTANCE]            = "distance";
    m[DbInt::FIELD_DISTRICT]            = "district";
    m[DbInt::FIELD_FILENAME]            = "filename";
    m[DbInt::FIELD_FLEET]               = "fleet";
    m[DbInt::FIELD_FROM]                = "from_id";
    m[DbInt::FIELD_FROM_TYPE]           = "from_type";
    m[DbInt::FIELD_GSSI]                = "gssi";
    m[DbInt::FIELD_ID]                  = "id";
    m[DbInt::FIELD_IH_ID]               = "ih_id";
    m[DbInt::FIELD_INC_ADDR1]           = "ih_add1";
    m[DbInt::FIELD_INC_ADDR2]           = "ih_add2";
    m[DbInt::FIELD_INC_CALLCARD_NO]     = "ih_callcard_no";
    m[DbInt::FIELD_INC_CATEGORY]        = "ih_cat";
    m[DbInt::FIELD_INC_CLOSED_DATE]     = "ih_closed_date";
    m[DbInt::FIELD_INC_DESC]            = "ih_desc";
    m[DbInt::FIELD_INC_DISP_DATE]       = "ih_dispatched_datetime";
    m[DbInt::FIELD_INC_LAT]             = "ih_latitude";
    m[DbInt::FIELD_INC_LON]             = "ih_longitude";
    m[DbInt::FIELD_INC_ONSCENE_DATE]    = "ih_onscene_datetime";
    m[DbInt::FIELD_INC_PRIORITY]        = "ih_priority";
    m[DbInt::FIELD_INC_RECVD_DATE]      = "ih_received_datetime";
    m[DbInt::FIELD_INC_RESOURCES]       = "ih_resources";
    m[DbInt::FIELD_INC_STATE]           = "ih_state";
    m[DbInt::FIELD_INC_STATUS]          = "ih_status";
    m[DbInt::FIELD_IR_ISSI]             = "ir_issi";
    m[DbInt::FIELD_IS_PUBLIC]           = "is_public";
    m[DbInt::FIELD_ISSI]                = "issi";
    m[DbInt::FIELD_ITEMCAT]             = "itemcat";
    m[DbInt::FIELD_LATITUDE]            = "latitude";
    m[DbInt::FIELD_LAYER]               = "layer";
    m[DbInt::FIELD_LOC_ISSI]            = "gps_issi";
    m[DbInt::FIELD_LOC_LATLONG]         = "gps_latlong";
    m[DbInt::FIELD_LOC_TIME]            = "gps_time";
    m[DbInt::FIELD_LOCK_HOLDER]         = "lock_holder";
    m[DbInt::FIELD_LONGITUDE]           = "longitude";
    m[DbInt::FIELD_MODIFIED_BY]         = "modified_by";
    m[DbInt::FIELD_MODIFIED_DATE]       = "modified_date";
    m[DbInt::FIELD_MON]                 = "monitor";
    m[DbInt::FIELD_NAME]                = "name";
    m[DbInt::FIELD_NOTES]               = "notes";
    m[DbInt::FIELD_OWNER]               = "owner";
    m[DbInt::FIELD_RTE_COST]            = "cost";
    m[DbInt::FIELD_RTE_MULTILINE]       = "multiline";
    m[DbInt::FIELD_SDS_MSG]             = "message";
    m[DbInt::FIELD_SHAPE]               = "shape";
    m[DbInt::FIELD_SHORTNAME]           = "shortName";
    m[DbInt::FIELD_SSI]                 = "ssi";
    m[DbInt::FIELD_START]               = "start";
    m[DbInt::FIELD_STATE]               = "state";
    m[DbInt::FIELD_STATE_CODE]          = "state_code";
    m[DbInt::FIELD_STATE_DESC]          = "state_desc";
    m[DbInt::FIELD_STATUSCODE_CODE]     = "sm_code";
    m[DbInt::FIELD_STATUSCODE_TEXT]     = "sm_text";
    m[DbInt::FIELD_STATUSCODE_TYPE]     = "sm_type";
    m[DbInt::FIELD_SUBDISTRICT]         = "subdistrict";
    m[DbInt::FIELD_TEXT]                = "text";
    m[DbInt::FIELD_TIME]                = "date";
    m[DbInt::FIELD_TO]                  = "to_id";
    m[DbInt::FIELD_TO_TYPE]             = "to_type";
    m[DbInt::FIELD_TX_PARTY]            = "tx_party";
    m[DbInt::FIELD_TX_PARTY_TYPE]       = "tx_party_type";
    m[DbInt::FIELD_TYPE]                = "type";
    m[DbInt::FIELD_TYPE_DESC]           = "type_desc";
    m[DbInt::FIELD_USER_ACTIVE]         = "active";
    m[DbInt::FIELD_USER_FAILED_COUNT]   = "failed_count";
    m[DbInt::FIELD_USER_FLEET]          = "fleet";
    m[DbInt::FIELD_USER_GROUP_ID]       = "group_id";
    m[DbInt::FIELD_USER_GROUP_NAME]     = "group_name";
    m[DbInt::FIELD_USER_NAME]           = "username";
    m[DbInt::FIELD_USER_PASSWD]         = "passwd";
    m[DbInt::FIELD_USER_ROLE]           = "users_role";
    m[DbInt::FIELD_VOIP_ID]             = "voip_id";
    m[DbInt::FIELD_ZOOM]                = "zoom";
    return m;
}

DbInt::FieldNameMapT DbInt::createActionMap()
{
    const string CFG("Config ");
    FieldNameMapT m;
    m[DbInt::ACT_ADD_GRPDATA]          = "Add Group Data";
    m[DbInt::ACT_ADD_INCIDENT]         = "Add Incident";
    m[DbInt::ACT_ADD_STSMSG]           = "Add Status Message";
    m[DbInt::ACT_ADD_SUBSDATA]         = "Add Subscriber Data";
    m[DbInt::ACT_ADD_USER_CLIENT]      = "Add client user";
    m[DbInt::ACT_ADD_USER_MOBILE]      = "Add mobile user";
    m[DbInt::ACT_ADD_USER_SERVER]      = "Add server user";
    m[DbInt::ACT_CFG]                  = CFG;
#ifdef AGW
    m[DbInt::ACT_AGW_RESTART]          = "AGW restart";
    m[DbInt::ACT_CFG_AGW_ISSI]         = CFG + "AGW ISSI";
    m[DbInt::ACT_CFG_AGW_LOGIN]        = CFG + "AGW Login";
    m[DbInt::ACT_CFG_AGW_MONSSIS]      = CFG + "AGW Monitor SSIs";
    m[DbInt::ACT_CFG_AGW_SERVERIP]     = CFG + "AGW Server IP";
    m[DbInt::ACT_CFG_AGW_SERVERPORT]   = CFG + "AGW Server Port";
    m[DbInt::ACT_CFG_AGW_SERVERSITENAME] = CFG + "AGW Server Site Name";
#endif
    m[DbInt::ACT_CFG_ALERT_SERVERIP]   = CFG + "Alert Server IP";
    m[DbInt::ACT_CFG_ALERT_SERVERPORT] = CFG + "Alert Server Port";
    m[DbInt::ACT_CFG_DB_ADDRESS]       = CFG + "Database IP";
    m[DbInt::ACT_CFG_DB_NAME]          = CFG + "Database Name";
    m[DbInt::ACT_CFG_DB_PASSWORD]      = CFG + "Database Password";
    m[DbInt::ACT_CFG_DB_PORT]          = CFG + "Database Port";
    m[DbInt::ACT_CFG_DB_USERNAME]      = CFG + "Database Username";
    m[DbInt::ACT_CFG_EM_SSI]           = CFG + "Emergency GSSI";
    m[DbInt::ACT_CFG_HELPDESKNUM]      = CFG + "Help Desk Number";
    m[DbInt::ACT_CFG_KEEPALIVE_PERIOD] = CFG + "Keepalive Period";
    m[DbInt::ACT_CFG_LOGFILE]          = CFG + "Log Filename";
    m[DbInt::ACT_CFG_LOGLEVEL]         = CFG + "Log Level";
    m[DbInt::ACT_CFG_LOGMAXCOUNT]      = CFG + "Log Max Count";
    m[DbInt::ACT_CFG_LOGMAXSIZE]       = CFG + "Log Max Size";
    m[DbInt::ACT_CFG_MAXCONNATTEMPTS]  = CFG + "Max Conn Attempts";
#ifdef PEER
    m[DbInt::ACT_CFG_PEER_IP]          = CFG + "Peer Server IPs";
    m[DbInt::ACT_CFG_PEER_PORT]        = CFG + "Peer Server Ports";
#endif
    m[DbInt::ACT_CFG_PORT]             = CFG + "Server Port";
    m[DbInt::ACT_CFG_REDUNDANCY_IP]    = CFG + "Redundant Server IP";
    m[DbInt::ACT_CFG_SAVE]             = "Save Configuration";
    m[DbInt::ACT_CFG_SDM_IP]           = CFG + "SDM IP";
    m[DbInt::ACT_CFG_SDM_PORT]         = CFG + "SDM Port";
    m[DbInt::ACT_CFG_SDM_RESTART]      = "Restart SDM Session";
    m[DbInt::ACT_CFG_STM_ID]           = CFG + "STM Login ID";
    m[DbInt::ACT_CFG_STM_IP]           = CFG + "STM IP";
    m[DbInt::ACT_CFG_STM_PORT]         = CFG + "STM Port";
    m[DbInt::ACT_CFG_STM_RESTART]      = "Restart STM Session";
    m[DbInt::ACT_CFG_TDP_PASSWORD]     = CFG + "Network System Password";
    m[DbInt::ACT_CFG_TDP_SERVERIP]     = CFG + "Network System Server IP";
    m[DbInt::ACT_CFG_TDP_SERVERPORT]   = CFG + "Network System Server Port";
    m[DbInt::ACT_CFG_TDP_USERNAME]     = CFG + "Network System Username";
    m[DbInt::ACT_CFG_TDP_USERNAMEGPS]  = CFG + "Network System GPS Username";
    m[DbInt::ACT_CFG_TRANS_TIMEOUT]    = CFG + "Transaction Timeout";
    m[DbInt::ACT_CFG_VOIP_SERVERIP]    = CFG + "VoIP Server IP";
    m[DbInt::ACT_CFG_VOIP_SERVERPORT]  = CFG + "VoIP Server Port";
    m[DbInt::ACT_DEL_GRPDATA]          = "Delete Group Data";
    m[DbInt::ACT_DEL_STSMSG]           = "Delete Status Message";
    m[DbInt::ACT_DEL_SUBSDATA]         = "Delete Subscriber Data";
    m[DbInt::ACT_DEL_USER_CLIENT]      = "Delete client user";
    m[DbInt::ACT_DEL_USER_MOBILE]      = "Delete mobile user";
    m[DbInt::ACT_DEL_USER_SERVER]      = "Delete server user";
    m[DbInt::ACT_EDIT_GRPDATA]         = "Edit Group Data";
    m[DbInt::ACT_EDIT_INCIDENT]        = "Edit Incident";
    m[DbInt::ACT_EDIT_STSMSG]          = "Edit Status Message";
    m[DbInt::ACT_EDIT_SUBSDATA]        = "Edit Subscriber Data";
    m[DbInt::ACT_EDIT_USER_CLIENT]     = "Edit client user";
    m[DbInt::ACT_EDIT_USER_MOBILE]     = "Edit mobile user";
    m[DbInt::ACT_EDIT_USER_SERVER]     = "Edit server user";
    m[DbInt::ACT_LOGIN_CLIENT]         = "Client login";
    m[DbInt::ACT_LOGIN_SERVER]         = "Server login";
    m[DbInt::ACT_LOGOUT_CLIENT]        = "Client logout";
    m[DbInt::ACT_LOGOUT_SERVER]        = "Server logout";
    m[DbInt::ACT_RESTART_TDPSESSION]   = "Restart Network System Session";
    m[DbInt::ACT_SERVER_START]         = "Start Server";
    m[DbInt::ACT_SERVER_STOP]          = "Stop Server";
    return m;
}

const string &DbInt::getFieldName(int id)
{
    if (sFieldNameMap.count(id) == 0)
        return sFieldNameMap[DbInt::FIELD_UNDEFINED];
    return sFieldNameMap[id];
}

DbInt::QResult::QResult(QueryResultT *res) : mResult(res)
{
    if (mResult == 0)
        assert("Bad param in DbInt::QResult::QResult" == 0);
}

DbInt::QResult::~QResult()
{
#ifndef NO_DB
    if (mResult != 0)
        PQclear(mResult);
#endif
}

int DbInt::QResult::getNumRows()
{
#ifndef NO_DB
    if (mResult != 0)
        return PQntuples(mResult);
#endif
    return 0;
}

bool DbInt::QResult::hasField(int field, int row)
{
#ifdef NO_DB
    return false;
#else
    if (mResult == 0)
        return false;
    int col = PQfnumber(mResult, getFieldName(field).c_str());
    return (col >= 0 && !PQgetisnull(mResult, row, col));
#endif
}

bool DbInt::QResult::getFieldValue(int field, string &value, int row)
{
#ifdef NO_DB
    return false;
#else
    if (mResult == 0)
        return false;
    int col = PQfnumber(mResult, getFieldName(field).c_str());
    if (col < 0)
        return false;
    value = PQgetvalue(mResult, row, col);
    return true;
#endif
}

bool DbInt::QResult::getFieldValue(int field, int &value, int row)
{
    string val;
    if (!getFieldValue(field, val, row))
        return false;
    value = Utils::fromString<int>(val);
    return true;
}

string DbInt::QResult::getFieldStr(int field, int row)
{
    string val;
    getFieldValue(field, val, row);
    return val;
}

bool DbInt::QResult::getFieldBool(int field, int row)
{
    //db query returns "t" for true, and "f" for false
    return (getFieldStr(field, row) == "t");
}

int DbInt::QResult::getReturnInt()
{
#ifndef NO_DB
    if (mResult == 0)
        return -1;
    string val(PQgetvalue(mResult, 0, 0));
    if (!val.empty())
        return Utils::fromString<int>(val);
#endif
    return -1;
}
