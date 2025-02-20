/**
 * SCAD/STM protocol message description.
 * Rules for adding: Type  - update createTypeMap().
 *                   Field - update createFieldNameMap().
 *                   Value - update getFieldValueString().
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: MsgSp.h 1888 2024-11-29 08:43:02Z rosnin $
 * @author Mohd Rashid
 * @author Mohd Rozaimi
 */
#ifndef MSGSP_H
#define MSGSP_H

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Utils.h"
#include "MsgValueBase.h"

class MsgSp
{
public:
    class Type
    {
    public:
        enum
        {
            SYS_KEEPALIVE              = 0,
            GPS_LOC                    = 1,
            CALL_SETUP                 = 2,
            CALL_PROCEEDING            = 3,
            CALL_CONNECT               = 4,
            CALL_RELEASE               = 5,
            CALL_TX_DEMAND             = 6,
            CALL_TX_GRANTED            = 7,
            CALL_TX_CEASED             = 8,
            CALL_DISCONNECT            = 9,
            STATUS                     = 10,
            STATUS_RPT                 = 11,
            SDS_TRANSFER               = 12,
            SDS_ACK                    = 13,
            SDS_RPT                    = 14,
            SDS_SHORT_RPT              = 15,
            CALL_ALERT                 = 16,
            CALL_CONNECT_ACK           = 17,
            CALL_TX_INTERRUPT          = 18,
            CALL_SSCF_FWD              = 19,
            CALL_FINISH                = 20,
            CALL_INFO                  = 21,

            GPS_MON_START              = 28,
            GPS_MON_STOP               = 29,

            MON_START                  = 30,
            MON_STOP                   = 31,
            MON_LOC                    = 32,
            MON_SETUP                  = 33,
            MON_CONNECT                = 34,
            MON_DISCONNECT             = 35,
            MON_TX_GRANTED             = 36,
            MON_TX_CEASED              = 37,
            MON_SDS                    = 38,
            MON_STATUS                 = 39,
            MON_SSCF_FWD               = 40,
            MON_SSIC_INCL              = 41,
            MON_SSIC_RELEASE           = 42,
            MON_GRP_ATTACH_DETACH      = 43,
            MON_SSDGNA_ASSIGN          = 44,
            MON_SSDGNA_DEASSIGN        = 45,
            MON_SSDGNA_DEFINE          = 46,
            MON_SSDGNA_DELETE          = 47,
            MON_SUBS_ASSIGN            = 48,
            MON_SUBS_DEASSIGN          = 49,
            MON_SUBS_DEFINE            = 50,
            MON_SUBS_DELETE            = 51,
            MON_SUBS_DESC              = 52,
            MON_SUBS_PERMISSION        = 53,
            MON_INFO                   = 54,

            SSIC_INVOKE                = 55,
            SSIC_CANCEL                = 56,
            SSIC_DISCONNECT            = 57,
            SSIC_INCL                  = 58,
            SSIC_INVOCATION_FAILURE    = 59,
            SSIC_RELEASE               = 60,

            LISTEN_CONNECT             = 61,
            LISTEN_DISCONNECT          = 62,
            LISTEN_RELEASE             = 63,

            SUBS_DATA                  = 64,
            SUBS_DATA_VPN              = 65,
            SUBS_DATA_DIV              = 66,
            SUBS_DATA_FLEET            = 67,
            SUBS_DATA_GROUP            = 68,

            STATUS_DATA                = 69,

            SSDGNA_QUERY_GRP           = 70,
            SSDGNA_DEFINE              = 71,
            SSDGNA_DELETE              = 72,
            SSDGNA_ASSIGN              = 73,
            SSDGNA_DEASSIGN            = 74,
            SSDGNA_GRP_LIST            = 75,
            SSDGNA_GSSI_LIST           = 76,
            SSDGNA_ISSI_LIST           = 77,
            SSDGNA_RESTART_ASG         = 78,

            SSCF_GENERAL_QUERY         = 90,
            SSCF_GENERAL_ACTIVATE      = 91,
            SSCF_QUERY                 = 92,
            SSCF_ACTIVATE              = 93,
            SSCF_DEACTIVATE            = 94,
            SSCF_SERVED_USERS_LIST     = 95,

            LOGIN                      = 100,
            LOGOUT                     = 101,
            PASSWORD                   = 102,
            CHANGE_PASSWORD            = 103,
            SYS_ERROR                  = 104,

            SSCAD_QUERY                = 110,
            SSCAD_CONFIG_DEFINE        = 111,
            SSCAD_RESTRICTION_DEFINE   = 112,
            SSCAD_INVOKE               = 113,

            INCIDENT_UPDATE            = 120,
            INCIDENT_LOCK              = 121,

            POI_DELETE                 = 130,
            POI_UPDATE                 = 131,

            VERSION_CLIENT             = 140,
            VERSION_MAP                = 141,
            MMS_TRANSFER               = 142,
            MMS_RPT                    = 143,
            BRANCH_DATA                = 144,

            //STM
            GRP_LINK                   = 300,
            VOIP_OPEN_CHANNEL          = 301,
            STATUS_ACK                 = 302,
            SUBS_REG                   = 303,
            SUBS_DEREG                 = 304,

            REMOTE_SERVER_DISCONNECTED = 900,
            REMOTE_SERVER_MAX_ATTEMPTS = 901,
            REMOTE_SERVER_TIMEOUT      = 902,

            VOIP_SERVER_REGISTERED     = 910,
            VOIP_SERVER_UNREGISTERED   = 911,

            UNDEFINED                  = 999
        };
    }; //class Type

    class Field
    {
    public:
        enum
        {
            TYPE                         = 1,
            MSG_ID                       = 2,
            MSG_ACK                      = 3,
            NETWORK_TYPE                 = 4,
            RESULT                       = 5,
            TIME                         = 6,
            CALL_ID                      = 7,
            MON_CALL_KEY                 = 8,
            NEW_CALL_ID                  = 9,

            CALLING_PARTY_TYPE           = 10,
            CALLING_PARTY                = 11,
            CALLING_PARTY_FLEET          = 12,
            CALLING_PARTY_VPN            = 13,
            CALLING_PARTY_CELL           = 14,
            CALLING_PARTY_GW             = 15,
            CALLED_PARTY_TYPE            = 16,
            CALLED_PARTY                 = 17,
            CALLED_PARTY_FLEET           = 18,
            CALLED_PARTY_VPN             = 19,
            CALLED_PARTY_GW              = 20,
            AI_ENCR_CTRL                 = 21,

            CALL_OWNERSHIP               = 22,
            CALL_PRIORITY                = 23,
            CALL_QUEUED                  = 24,
            CALL_STATUS                  = 25,
            CALL_TIMEOUT                 = 26,
            CALL_TIMEOUT_SETUP           = 27,
            CIRCUIT_MODE_TYPE            = 28,
            COMM_TYPE                    = 29,
            ENCR_FLAG                    = 30,
            ENCR_CTRL                    = 31,
            HOOK_METHOD                  = 32,
            SIMPLEX_DUPLEX               = 33,
            CLIR_CTRL                    = 34,
            REQ_TO_TX                    = 35,
            TX_DEMAND_PRIORITY           = 36,
            TX_GRANT                     = 37,
            TX_REQ_PERMISSION            = 38,
            TX_PARTY_TYPE                = 39,
            TX_PARTY                     = 40,
            TX_PARTY_FLEET               = 41,
            TX_PARTY_VPN                 = 42,
            TX_PARTY_CELL                = 43,
            TX_PARTY_GW                  = 44,

            GRP_TYPE                     = 48,
            GSSI                         = 49,
            GRP_NAME                     = 50,
            DGNA_GRP                     = 51,
            DGNA_GRP_TYPE                = 52,
            DGNA_GRP_LIST                = 53,
            DGNA_GSSI_LIST               = 54,
            DGNA_ISSI_LIST               = 55,
            GRP_ATT_MODE                 = 56,
            CLASS_OF_USAGE               = 57,
            GRP_AI_ENCR_MODE             = 58,
            ASG_DUR                      = 59,
            LIFETIME_DEFAULT             = 60,
            IDLE_TIME                    = 61,
            ASG_REP_RATE                 = 62,
            ACK_REQ_FR_AFFECTED_USER     = 63,
            STORE_FLAG                   = 64,
            MAX_CALL_PRIORITY            = 65,
            MIN_CALL_PRIORITY            = 66,
            DGNA_STATUS_ISSI_LIST        = 67,
            ASG_DEASSIGN_STATUS          = 68,
            AFFECTED_USER_TYPE           = 69,
            AFFECTED_USER                = 70,
            MSG_NUM                      = 71,
            TOTAL_MESSAGES               = 72,
            DISCONNECT_CAUSE             = 73,

            LOCATION_LAT                 = 74,
            LOCATION_LONG                = 75,
            LOCATION_TIME                = 76,
            LOCATION_VALID               = 77,
            LOCATION_ACCURACY            = 78,
            LOCATION_VELOCITY            = 79,
            LOCATION_DIRECTION           = 80,
            LOCATION_ALTITUDE            = 81,

            STATUS_CODE                  = 86,
            STATUS_CODE_LIST             = 87,
            STATUS_TYPE_LIST             = 88,
            STATUS_TEXT_LIST             = 89,

            SHORT_DATA_TYPE              = 90,
            PROTOCOL_ID                  = 91,
            DELIVERY_RPT_REQ             = 92,
            ACK_REQUIRED                 = 93,
            STORAGE                      = 94,
            MSG_REF                      = 95,
            VALIDITY_PERIOD              = 96,
            SHORT_RPT_TYPE               = 97,
            USER_DATA_LENGTH             = 98,
            USER_DATA                    = 99,
            DELIVERY_STATUS              = 100,
            SVC_SELECT_SHORT_FORM_RPT    = 101,
            USER_DEFINED_DATA_1          = 102,
            USER_DEFINED_DATA_2          = 103,
            USER_DEFINED_DATA_3          = 104,
            USER_DEFINED_DATA_4          = 105,

            CELL                         = 110,
            LOC_UPDATE_TYPE              = 111,
            GRP_ATTACH_DETACH_MODE       = 112,
            GRP_ID                       = 113,
            TIME_IN_Q                    = 114,
            TIME_IN_TRAFFIC              = 115,
            SUBS_CONTAINER_TYPE          = 116,
            SUBS_CONTAINER_ID            = 117,
            SUBS_CONTENT_TYPE            = 118,
            SUBS_CONTENT_ID              = 119,
            SUBS_PERMISSION              = 120,

            INCL_TYPE                    = 130,
            INCL_PARTY_TYPE              = 131,
            INCL_PARTY                   = 132,
            INCL_PARTY_FLEET             = 133,
            INCL_PARTY_VPN               = 134,
            INCL_PARTY_CELL              = 135,
            INCL_PARTY_GW                = 136,
            INVOCATION_FAIL_CAUSE        = 137,

            PARTY_SELECT                 = 150,

            SSCF_ACTIVE                  = 160,
            SSCF_QUERY_TYPE              = 161,
            SSCF_DEACTIVATION_TYPE       = 162,
            SSCF_REJECT_CAUSE            = 163,
            SSCF_STATE                   = 164,
            CONFIGURED_CALL_FWD_TYPE     = 165,
            DIVERTED_BASIC_SVC_INFO      = 166,
            INVOKED_CALL_FWD_TYPE        = 167,
            SERVED_USER_TYPE             = 168,
            SERVED_USER                  = 169,
            SERVED_USER_FLEET            = 170,
            SERVED_USER_VPN              = 171,
            SERVED_USER_GW               = 172,
            SERVED_USER_LIST             = 173,
            FWD_TO_USER_TYPE             = 174,
            FWD_TO_USER                  = 175,
            FWD_TO_USER_FLEET            = 176,
            FWD_TO_USER_VPN              = 177,
            FWD_TO_USER_GW               = 178,
            FWD_ADDR_TYPE                = 179,
            FWD_ADDR                     = 180,
            FWD_ADDR_GW                  = 181,

            VPN                          = 190,
            DIVISION                     = 191,
            FLEET                        = 192,
            ISSI                         = 193,
            EXTN                         = 194,
            DIVISION_LIST                = 195,
            FLEET_LIST                   = 196,
            SSI_LIST                     = 197,
            GRP_LIST                     = 198,
            ISSI_LIST                    = 199,
            EXTN_LIST                    = 200,
            LINE_LIST                    = 201,
            TERMINAL_TYPE                = 202,

            TIMESTAMP                    = 210,
            CHECKSUM                     = 211,
            DESC                         = 212,
            DESC_LIST                    = 213,
            SUPPORTNUM                   = 214,
            ORGANIZATION                 = 215,
            ID                           = 216,
            VERSION                      = 217,
            PORT                         = 218,

            INVALID_MSG_TYPE             = 220,
            INVALID_FIELD                = 221,
            INVALID_FIELD_VALUE          = 222,

            FILE_LIST                    = 225,
            FILE_PATH                    = 226,
            FILE_SIZE                    = 227,

            USERNAME                     = 230,
            USER_TYPE                    = 231,
            PASSWORD                     = 232,
            NEW_PASSWORD                 = 233,
            CHALLENGE                    = 234,
            KEEPALIVE_PERIOD             = 235,
            MAC_ADDRESSES                = 236,
            MAX_MONITORING_GSSI          = 237,
            MAX_MONITORING_ISSI          = 238,
            RETRY_COUNT                  = 239,

            SSCAD_FLAGS                  = 240,
            SSCAD_INTERCEPT_CAUSE        = 241,
            SSCAD_INVOKE_CMD             = 242,
            SSCAD_RESTRICTED_DIR         = 243,
            SSCAD_RESTRICTED_PEER_TYPE   = 244,
            SSCAD_RESTRICTED_PEER        = 245,
            SSCAD_RESTRICTED_SVC         = 246,
            SSCAD_RESTRICTION_DEFINE_CMD = 247,
            SSCAD_RESULT_SEQ_NUM         = 248,
            RESTRICTED_USER_TYPE         = 249,
            RESTRICTED_USER              = 250,

            DB_ADDRESS                   = 260,
            DB_NAME                      = 261,
            DB_PORT                      = 262,
            DB_USERNAME                  = 263,
            DB_PASSWORD                  = 264,

            INCIDENT_ID                  = 270,
            LOCK_ACTION                  = 271,

            VOIP_GW                      = 280,
            VOIP_AUDIO_RTP_LPORT         = 281,
            VOIP_AUDIO_RTP_PORT          = 282,
            VOIP_VIDEO_RTP_LPORT         = 283,
            VOIP_VIDEO_RTP_PORT          = 284,
            VOIP_SSRC                    = 285,
            VOIP_AUDIO_RTP_LKEY          = 286,
            VOIP_AUDIO_RTP_KEY           = 287,
            VOIP_VIDEO_RTP_LKEY          = 288,
            VOIP_VIDEO_RTP_KEY           = 289,

            //STM
            AREA_SELECT                  = 400,
            CTRL_ENTITY                  = 401,
            INTR_PARTY                   = 402,
            INTR_PARTY_TYPE              = 403,
            KPI_MCI                      = 404,
            MC_SUBS_REQ                  = 405,
            NOTIF_IND                    = 406,
            QOS                          = 407,
            SLOTS_PER_FRAME              = 408,
            SPEECH_SVC                   = 409,
            SUBS_REG_TYPE                = 410,

            UNDEFINED                    = 999
        };
    }; //class Field

    class Value : public MsgValueBase
    {
    public:
        enum
        {
            //Disconnect Cause
            //values must be greater than the max DC in MsgValueBase
            DC_CALL_COMPLETED_ELSEWHERE = 200,

            //Identity Type - Database table d_identity_type must match this
            IDENTITY_TYPE_UNKNOWN    = 0,
            IDENTITY_TYPE_ISSI       = 1,
            IDENTITY_TYPE_GSSI       = 2,
            IDENTITY_TYPE_PSTN       = 3,
            IDENTITY_TYPE_EXTN       = 4,
            IDENTITY_TYPE_VPN        = 5,
            IDENTITY_TYPE_DIVISION   = 6,
            IDENTITY_TYPE_FLEET      = 7,
            IDENTITY_TYPE_DISPATCHER = 8,
            IDENTITY_TYPE_MOBILE     = 9,
            IDENTITY_TYPE_SNA        = 10,
            IDENTITY_TYPE_TSI        = 11,

            //Location Validity
            LOCATION_VALID_NO  = 0,
            LOCATION_VALID_YES = 1,

            LOCK_ACTION_LOCK   = 0,
            LOCK_ACTION_UNLOCK = 1,

            //Msg Id - limit to 6 digits
            MSG_ID_MIN = 1,
            MSG_ID_MAX = 999999,

            //Network Type - non-zero values only
            NETWORK_TYPE_TETRA  = 1,
            NETWORK_TYPE_TETRA2 = 2,
            NETWORK_TYPE_TETRA3 = 3,

            //Result
            RESULT_OK                       = 0,
            RESULT_REJECTED                 = 1,
            RESULT_REJECTED_SECURITY        = 2,
            RESULT_NOT_AUTHORIZED           = 3,
            RESULT_UNDEFINED_ERROR          = 4,
            RESULT_TEMP_NOT_AVAILABLE       = 5,
            RESULT_TIMEOUT                  = 6,
            RESULT_INVALID_PASSWORD         = 7,
            RESULT_INVALID_USER             = 8,
            RESULT_ALREADY_LOGGED_IN        = 9,
            RESULT_TX_PROBLEM               = 10,
            RESULT_NETW_NOT_REGISTERED      = 11,
            RESULT_RETRY                    = 12,
            RESULT_NO_ACTIVE_CALL           = 13,
            RESULT_NO_VOIP_CHAN             = 14,
            RESULT_CALL_HAS_ENDED           = 15,
            RESULT_DUPLICATE_REQ            = 16,
            RESULT_DB_LINK_ERROR            = 17,
            RESULT_LIC_EXPIRED              = 18,
            RESULT_LIC_MAX                  = 19,
            RESULT_LIC_INVALID_MAC          = 20,
            RESULT_INVALID_CLIENT           = 21,
            RESULT_INVALID_CLIENT_TYPE      = 22,
            RESULT_TIMEOUT_VOIP             = 23,
            RESULT_SVR_SHUTTING_DOWN        = 24,
            RESULT_OUT_OF_CONTEXT           = 25,
            RESULT_INACTIVE_USER            = 26,
            RESULT_BUSY_USER                = 27,

            RESULT_SSI_ALREADY_ASSIGNED     = 30,
            RESULT_GSSI_NOT_ASSIGNED        = 31,
            RESULT_MAX_GROUP_EXCEEDED       = 32,
            RESULT_MNEMONIC_TOO_LONG        = 33,
            RESULT_ERROR_GETTING_INFO       = 34,
            RESULT_ERROR_GETTING_DGNA_GRP   = 35,
            RESULT_GRP_NOT_EXIST            = 36,
            RESULT_ISSI_NOT_REACHABLE       = 37,
            RESULT_INVALID_COU              = 38,
            RESULT_INVALID_VPN              = 39,
            RESULT_INVALID_GSSI             = 40,
            RESULT_INVALID_ISSI             = 41,
            RESULT_INVALID_CALL_ID          = 42,
            RESULT_QUERY_TYPE_NOT_SUPPORTED = 43,
            RESULT_TYPE_NOT_SUPPORTED       = 44,
            RESULT_WRONG_DEST               = 45,
            RESULT_GRP_EMPTY                = 46,

            RESULT_DGNA_GRP_EXIST                  = 50,
            RESULT_DGNA_GRP_EMPTY                  = 51,
            RESULT_DGNA_GRP_NOT_EMPTY              = 52,
            RESULT_DGNA_NOT_ALLOWED                = 53,
            RESULT_DGNA_NO_ELEMENT                 = 54,
            RESULT_DGNA_MAX_REACHED                = 55,
            RESULT_DGNA_REQ_FAILED                 = 56,
            RESULT_DGNA_MS_SUPPORT_NO              = 57,
            RESULT_DGNA_LACK_INFO                  = 58,
            RESULT_DGNA_INVALID_AFFECTED_USER_TYPE = 59,
            RESULT_DGNA_INVALID_ASG_DUR            = 60,
            RESULT_DGNA_INVALID_ASG_REP_RATE       = 61,
            RESULT_DGNA_INVALID_GRP_TYPE           = 62,
            RESULT_DGNA_INVALID_IDLE_TIME          = 63,
            RESULT_DGNA_INVALID_LIFETIME_DEFAULT   = 64,
            RESULT_DGNA_INVALID_MAX_CALL_PRIORITY  = 65,
            RESULT_DGNA_INVALID_MIN_CALL_PRIORITY  = 66,
            RESULT_DGNA_MISSING_ASG_DUR            = 67,
            RESULT_DGNA_MISSING_ASG_REP_RATE       = 68,
            RESULT_DGNA_MISSING_IDLE_TIME          = 69,
            RESULT_DGNA_MISSING_LIFETIME_DEFAULT   = 70,
            RESULT_DGNA_PROCEEDING                 = 71,

            RESULT_SS_INVALID_TYPE       = 80,
            RESULT_SS_SVC_NOT_SUBSCRIBED = 81,
            RESULT_SS_NOT_AVAILABLE      = 82,

            RESULT_MON_INVALID_NETW      = 90,
            RESULT_MON_NETW_NOT_ENABLED  = 91,

            RESULT_GRP_MON_FAILED        = 95,
            RESULT_GRP_MON_INVALID_GSSI  = 96,
            RESULT_GRP_MON_NOT_ALLOWED   = 97,
            RESULT_GRP_MON_MAX_REACHED   = 98,
            RESULT_GRP_MON_NOT_SUPPORTED = 99,

            //see isMmsError() when changing here
            RESULT_MMSERR_INVALID        = 120,
            RESULT_MMSERR_FILE           = 121,
            RESULT_MMSERR_CONNECT        = 122,
            RESULT_MMSERR_UPLOAD         = 123,
            RESULT_MMSERR_DOWNLOAD       = 124,
            //permanent failure - file no longer on server
            RESULT_MMSERR_DOWNLOAD_PERM  = 125
        };

        static const char ENDL               = '\n';
        static const char LIST_DELIMITER     = ',';  //inside list of values
        static const char PAIR_DELIMITER     = '-';  //inside a value pair
        static const char SIP_DELIMITER      = ',';  //in MsgSip
        //between group IDs which contain a comma-separated list
        static const char GID_LIST_DELIMITER = ';';
    }; //class Value

    typedef std::vector<std::vector<int> > NestedListT;

    //# bytes for message length at the start of a message stream
    static const int LEN_SIZE = 2;

    /**
     * Constructor. Sets the message creation timestamp.
     *
     * @param[in] type The message type.
     */
    MsgSp(int type = -1);

    /**
     * Copy constructor. Either copies or resets the message creation timestamp.
     *
     * @param[in] src           The source.
     * @param[in] keepTimestamp true to copy the source timestamp.
     */
    MsgSp(const MsgSp &src, bool keepTimestamp = false);

    MsgSp &operator=(const MsgSp &src);

    friend std::ostream &operator<<(std::ostream &os, const MsgSp &msg);

    /**
     * Checks whether the message type is valid.
     *
     * @return true if valid.
     */
    bool isValid() const;

    /**
     * Checks whether a message field exists.
     *
     * @param[in] key The field key.
     * @return true if exists.
     */
    bool hasField(int key) const;

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @return *this.
     */
    MsgSp &addField(int key, const std::string &value);

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @return *this.
     */
    MsgSp &addField(int key, int value);

    /**
     * Adds a message field.
     *
     * @param[in] keyAndValue The field key and value pair.
     * @return *this.
     */
    MsgSp &addField(const std::pair<int, std::string> &keyAndValue);

    /**
     * Appends value to a message field using a delimiter.
     * If the field does not yet exist, just sets it to the value without using
     * the delimiter.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @param[in] delim The delimiter.
     * @return *this.
     */
    MsgSp &appendField(int                key,
                       const std::string &value,
                       char               delim = Value::GID_LIST_DELIMITER);

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @tparam    T     The value type.
     * @return *this.
     */
    template<class T>
    MsgSp &addField(int key, T value);

    /**
     * Adds a message field containing a list of values.
     *
     * @param[in] key    The field key.
     * @param[in] values The list of values.
     * @return *this.
     */
    template<class T>
    MsgSp &addField(int key, const std::set<T> &values);

    /**
     * Adds a message field containing a list of values.
     *
     * @param[in] key    The field key.
     * @param[in] values The list of values.
     * @return *this.
     */
    template<class T>
    MsgSp &addField(int key, const std::vector<T> &values);

    /**
     * Removes a field.
     *
     * @param[in] key The field key. Does nothing if the key does not exist.
     * @return *this.
     */
    MsgSp &removeField(int key);

    /**
     * Clears the message fields and resets the creation time to now, and
     * optionally changes the type.
     *
     * @param[in] type The new type, if any.
     * @return *this.
     */
    MsgSp &reset(int type = -1);

    /**
     * Gets a field string.
     *
     * @param[in] key The field key.
     * @return Field string if the key exists, otherwise empty string.
     */
    std::string getFieldString(int key) const;

    /**
     * Gets a field numeric value.
     *
     * @param[in] key The field key.
     * @return Field value if the key exists and has a numeric value,
     *         otherwise Value::UNDEFINED.
     */
    int getFieldInt(int key) const;

    /**
     * Gets a field list of values.
     *
     * @param[in]  key    The field key.
     * @param[out] values The values if found, otherwise not modified.
     * @return true if values found.
     */
    bool getFieldVals(int key, NestedListT &values) const;

    /**
     * Gets a field numeric value.
     *
     * @param[in]  key   The field key.
     * @param[out] value The value if found, otherwise not modified.
     * @tparam     T     The value type.
     * @return true if value found.
     */
    template<class T>
    bool getFieldVal(int key, T &value) const;

    /**
     * Gets a field list of values.
     *
     * @param[in]  key    The field key.
     * @param[out] values The values if found, otherwise not modified.
     * @tparam     T      The values type.
     * @return true if values found.
     */
    template<class T>
    bool getFieldVals(int key, std::vector<T> &values) const;

    /**
     * Gets a field map of values.
     *
     * @param[in]  key    The field key.
     * @param[out] values The values if found, otherwise not modified.
     * @tparam     T      The values type.
     * @return true if values found.
     */
    template<class T>
    bool getFieldVals(int key, std::map<int, T> &values) const;

    /**
     * Gets the message timestamp.
     *
     * @param[in] inclMs true to include the milliseconds part.
     * @return The timestamp string.
     */
    std::string getTimestamp(bool inclMs = true) const;

    /**
     * Gets a field string in the form:
     *     <Name> <Value> [<Description>]
     * E.g.:
     *   With description:    "Result 8 INVALID_USER"
     *                        "Called-Party-Type 1 ISSI"
     *   Without description: "Calling-Party 3201234"
     *
     * @param[in] key   The field key.
     * @param[in] value The field value, if available. Otherwise taken from the
     *                  message content.
     * @return The field value string.
     */
    std::string getFieldValueString(int                key,
                                    const std::string &value = "") const;

    /**
     * Converts a class object to string. Fields are shown in full name.
     *
     * @return The string representation of the object.
     */
    std::string toString() const;

    /**
     * Serializes a class object for transmission. Fields are shown in value.
     * Encrypts the serialized data if required.
     *
     * @param[in] key Encryption key, if required. Recipient must decrypt with
     *                the same key.
     * @return The serialized object.
     */
    std::string serialize(const std::string &key = "") const;

    /**
     * Serializes a class object for transmission in SIP, using
     * Value::SIP_DELIMITER as field delimiter instead of Value::ENDL, with no
     * delimiter at the end. Fields are shown in value.
     * Caution: Cannot be used for a message with fields containing
     *          Value::LIST_DELIMITER.
     *
     * @return The serialized string.
     */
    std::string sipSerialize() const;

    /**
     * Does message serialization and gets the actual data bytes.
     * Encrypts the data if required.
     *
     * @param[out] len The number of data bytes.
     * @param[in]  key Encryption key, if required. Recipient must decrypt with
     *                 the same key.
     * @return The data bytes. Caller must destroy with delete [].
     */
    char *getBytes(int &len, const std::string &key = "") const;

    /**
     * Gets the message name.
     *
     * @return The message name if exists, otherwise "UNDEFINED".
     */
    const std::string &getName() const;

    /**
     * Gets the message name and ID as "<name> <ID>".
     * The name is as from getTypeName(), and ID from getMsgId().
     *
     * @return The message name and ID.
     */
    std::string getNameAndId() const;

    MsgSp &setType(int type)
    {
        mType = type;
        return *this;
    }

    int getType() const { return mType; }

    /**
     * Sets a unique message ID from the running number.
     *
     * @return The message ID.
     */
    int setMsgId();

    /**
     * Gets the message ID.
     *
     * @return The message ID if present, otherwise 0.
     */
    int getMsgId() const;

    /**
     * Sets the SDS message text.
     *
     * @param[in] text   The message text.
     * @param[in] coding The text coding type. See MsgSp::Value::TEXT_CODING_*.
     */
    void setSdsText(const std::string &text,
                    int                coding =
                                              Value::TEXT_CODING_8859_1_LATIN1);

    /**
     * Gets the SDS message text. Caller must have checked for the correct
     * message type first. For SDS containing location data (based on the
     * protocol ID) the text is formed as "<latitude>,<longitude>".
     *
     * @param[out] text The message text for type-4 SDS. Otherwise the raw
     *                  hexadecimal user data.
     * @return Value::UNDEFINED for location data.
     *         Otherwise the text coding type, or -1 on error.
     *         See MsgSp::Value::TEXT_CODING_*.
     */
    int getSdsText(std::string &text) const;

    /**
     * Converts the User-Data text into transmission format.
     */
    void setUserText();

    /**
     * Converts the User-Data from transmission format.
     */
    std::string getUserText();

#if defined(SERVERAPP) && defined(APP_STM)
    /**
     * Gets information from the SDS type 4 user data and adds to the relevant
     * fields. This is for received SDS_TRANSFER from network.
     * May change the message type to SDS_RPT based on the user data.
     *
     * @return false for SDS type 4 with invalid user data length, which
     *         indicates invalid message. true otherwise.
     */
    bool getSdsUserData();

    /**
     * Gets information from the relevant fields into the SDS type 4 user data.
     * This is for sending out SDS_TRANSFER/SDS_RPT to network.
     *
     * @param[in]  ref  The 8-bit message reference, replacing the one already
     *                  here.
     * @param[out] data The user data. Must be empty on entry.
     * @return The user data length in bits. 0 for invalid message type.
     */
    int getSdsUserData(int ref, std::string &data) const;

    /**
     * Sets information from the relevant fields into the SDS type 4 user data.
     * This is for sending out SDS_TRANSFER/SDS_RPT to network.
     */
    void setSdsUserData();

    /**
     * Changes a GPS_LOC message to SDS_TRANSFER and sets the location
     * information in the SDS type 4 user data.
     * Generates LIP Long format if location information contains altitude.
     * Otherwise generates LIP Short format.
     * This is for sending out client location update to network.
     *
     * @return true if successful. false for invalid message.
     */
    bool setSdsLocData();
#endif //SERVERAPP && APP_STM

    /**
     * Checks whether this message is for a final response or an interim
     * response. For example, SSDGNA_GRP_LIST is the final response only when
     * MSG_NUM == TOTAL_MESSAGES.
     *
     * @return true if final response.
     */
    bool isFinalResponse() const;

    /**
     * Checks for success as indicated by the RESULT field, which either:
     *   -is not present,
     *   -has value RESULT_OK.
     *
     * @return true if successful.
     */
    bool isResultSuccessful() const;

    /**
     * Checks for successful SDS transmission as indicated by the
     * DELIVERY_STATUS field for SDS_RPT or SHORT_RPT_TYPE field for
     * SDS_SHORT_RPT.
     *
     * @return true if successful.
     */
    bool isSdsSuccessful() const;

    /**
     * Checks for MMS error in RESULT.
     *
     * @return true for MMS error.
     */
    bool hasMmsError() const;

    /**
     * Gets the message priority for queueing.
     * This is the CALL_PRIORITY if the field exists.
     * Otherwise TX_DEMAND_PRIORITY if it exists.
     * A status message with code STATUS_CODE_EMERGENCY has the highest priority
     * equal to emergency call.
     * Other messages get the default 0.
     *
     * @return The priority. Higher value for higher priority.
     */
    int getPriority() const;

    /**
     * Compares the message priority against the given message, only if
     * -both messages are of the same type (for CALL_TX_DEMAND, only within the
     *  same call), or
     * -one of the messages is STATUS.
     * Otherwise always considers them equal.
     *
     * @param[in] msg The comparing message.
     * @return <0 = Lower priority for this message (i.e. higher for msg).
     *          0 = Equal.
     *         >0 = Higher.
     */
    int comparePriority(const MsgSp &msg) const;

    /**
     * Gets a message type name.
     *
     * @param[in] type The message type.
     * @return The type name if exists, otherwise "UNDEFINED".
     */
    static const std::string &getTypeName(int type);

    /**
     * Gets a field name.
     *
     * @param[in] id The field ID.
     * @return The field name if the ID exists, otherwise "UNDEFINED".
     */
    static const std::string &getFieldName(int id);

    /**
     * Creates an encryption key from a base key by calculating the base
     * checksum and adding to each base character.
     *
     * @param[in] base The base key.
     * @return The new key.
     */
    static std::string getKey(std::string base);

    /**
     * Gets the message length from the first LEN_SIZE bytes of a data string.
     *
     * @param[in] str The data string.
     * @return The message length, or a value beyond the range of LEN_SIZE bytes
     *         if the string length is insufficient.
     */
    static int getMsgLen(const std::string &str);

    /**
     * Parses a message string into a message object.
     * Decrypts the string if required.
     *
     * @param[in] str The message string.
     * @param[in] key Decryption key, if required. Sender must have
     *                encrypted using the same key.
     * @return The message object, or 0 on failure. Caller takes ownership
     *         of the created object, and is responsible for deleting it.
     */
    static MsgSp *parse(const std::string &str, const std::string &key = "");

    /**
     * Scrambles a string into a hexadecimal string using a key.
     *
     * @param[in] str The string.
     * @param[in] key The key.
     * @return The scrambled hexadecimal string. Empty string if str is empty.
     */
    static std::string hexScramble(const std::string &str,
                                   const std::string &key);

    /**
     * Unscrambles a hexadecimal string using a key.
     *
     * @param[in] str The hexadecimal string.
     * @param[in] key The key used when scrambling the original string.
     * @return The unscrambled string. Empty string if str is empty.
     */
    static std::string hexUnscramble(const std::string &str,
                                     const std::string &key);

    /**
     * Scrambles or unscrambles a string.
     *
     * @param[in] str     The string.
     * @param[in] forward true to scramble, false to unscramble.
     * @return The processed string.
     */
    static std::string scramble(const std::string &str, bool forward);

    /**
     * Checks whether a RESULT value is for MMS error.
     *
     * @param[in] result The value.
     * @return true for MMS error.
     */
    static bool isMmsError(int result)
    {
        return (result >= Value::RESULT_MMSERR_INVALID &&
                result <= Value::RESULT_MMSERR_DOWNLOAD_PERM);
    }

private:
    //holds information for a particular message type
    struct MsgInfo
    {
        MsgInfo() {}
        MsgInfo(std::string theName, std::string theVersion) :
            name(theName), version(theVersion) {}

        std::string name;
        std::string version;
    };

    typedef std::map<int, std::string> FieldsMapT;
    typedef std::map<int, MsgInfo>     TypeMapT;
    typedef std::map<int, std::string> FieldNameMapT;

    int         mType;         //message type
    std::string mTimestampStr; //creation time
    FieldsMapT  mFields;       //field values

    static TypeMapT      sTypeMap;      //message type details
    static FieldNameMapT sFieldNameMap; //field names

    /**
     * Encrypts or decrypts a message string.
     *
     * @param[in] str     The input string.
     * @param[in] key     Encryption/decryption key.
     * @param[in] encrypt true to encrypt.
     * @return The encrypted cipher text or decrypted plain text.
     */
    static std::string crypt(const std::string &str,
                             const std::string &key,
                             bool               encrypt);

    /**
     * Creates a mapping of type values to string. Used to initialize the static
     * map member.
     *
     * @return The map.
     */
    static TypeMapT createTypeMap();

    /**
     * Creates a mapping of field values to string. Used to initialize the
     * static map member.
     *
     * @return The map.
     */
    static FieldNameMapT createFieldNameMap();
};

template<class T>
MsgSp &MsgSp::addField(int key, T value)
{
    mFields[key] = Utils::toString(value);
    return *this;
}

template<class T>
MsgSp &MsgSp::addField(int key, const std::set<T> &values)
{
    if (!values.empty())
        mFields[key] = Utils::toString(values, Value::LIST_DELIMITER);
    return *this;
}

template<class T>
MsgSp &MsgSp::addField(int key, const std::vector<T> &values)
{
    if (!values.empty())
        mFields[key] = Utils::toString(values, Value::LIST_DELIMITER);
    return *this;
}

template<class T>
bool MsgSp::getFieldVal(int key, T &value) const
{
    auto it = mFields.find(key);
    return (it != mFields.end() &&
            Utils::fromString<T>(it->second, value));
}

template<class T>
bool MsgSp::getFieldVals(int key, std::vector<T> &values) const
{
    auto it = mFields.find(key);
    return (it != mFields.end() &&
            Utils::fromString<T>(it->second, values, Value::LIST_DELIMITER));
}

template<class T>
bool MsgSp::getFieldVals(int key, std::map<int, T> &values) const
{
    auto it = mFields.find(key);
    return (it != mFields.end() &&
            Utils::fromString<T>(it->second, values, Value::LIST_DELIMITER,
                                 Value::PAIR_DELIMITER));
}
#endif //MSGSP_H
