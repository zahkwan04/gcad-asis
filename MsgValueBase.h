/**
 * A base class for message field values.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: MsgValueBase.h 1718 2023-07-11 09:05:17Z hazim.rujhan $
 * @author Mohd Rashid
 */
#ifndef MSGVALUEBASE_H
#define MSGVALUEBASE_H

class MsgValueBase
{
public:
    enum
    {
        //Ack Requested From Affected Users
        ACK_REQ_FAU_NO  = 0,
        ACK_REQ_FAU_YES = 1,

        //Ack Required
        ACK_REQ_NO  = 0,
        ACK_REQ_YES = 1,

        //AI Encryption Control
        AI_ENCR_CTRL_CLEAR = 0,
        AI_ENCR_CTRL_AIR   = 1,

        //Assign Deassign Status
        ASGD_ASSIGNED         = 0,
        ASGD_DEASSIGNED       = 1,
        ASGD_PENDING_ASSIGN   = 2,
        ASGD_PENDING_DEASSIGN = 3,
        ASGD_NOT_SUPPORTED    = 4,

        //Assignment Duration
        ASG_DUR_PREDEFINED = 0,
        ASG_DUR_SEC_10     = 1,
        ASG_DUR_SEC_30     = 2,
        ASG_DUR_SEC_100    = 3,
        ASG_DUR_SEC_200    = 4,
        ASG_DUR_SEC_400    = 5,
        ASG_DUR_SEC_1000   = 6,
        ASG_DUR_SEC_2000   = 7,
        ASG_DUR_IDLE       = 8,

        //Assignment Repeat Rate
        ASG_REP_NO     = 0,
        ASG_REP_LOW    = 1,
        ASG_REP_NORMAL = 2,
        ASG_REP_HIGH   = 3,

        //Call ID limits
        CALL_ID_MIN = 1,
        CALL_ID_MAX = 16383,

        //Call Ownership
        CALL_OWNER_NO      = 0, //or indiv normal call
        CALL_OWNER_YES     = 1, //or indiv amalgamated call
        CALL_OWNER_UNKNOWN = 2,

        //Call Priority
        //Maximum Call Priority
        //Minimum Call Priority
        CALL_PRIORITY_UNDEFINED              = 0,
        CALL_PRIORITY_LOWEST                 = 1,
        CALL_PRIORITY_PREEMPTIVE_1           = 12,
        CALL_PRIORITY_PREEMPTIVE_2           = 13,
        CALL_PRIORITY_PREEMPTIVE_3           = 14,
        CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY = 15,

        //Call Queued
        CALL_QUEUED_NO  = 0,
        CALL_QUEUED_YES = 1,

        //Call Related DGNA Creation
        CRDC_NO  = 0,
        CRDC_YES = 1,

        //Call Status
        CALL_STAT_PROCEEDING        = 0,
        CALL_STAT_QUEUED            = 1,
        CALL_STAT_SUBSCR_PAGED      = 2,
        CALL_STAT_CONTINUE          = 3,
        CALL_STAT_HANG_TIME_EXPIRED = 4,
        //MCIP reserved 5-7

        //Call Timeout
        CALL_TIMEOUT_INFINITE = 0,
        CALL_TIMEOUT_SEC_30   = 1,
        CALL_TIMEOUT_SEC_45   = 2,
        CALL_TIMEOUT_SEC_60   = 3,
        CALL_TIMEOUT_MIN_2    = 4,
        CALL_TIMEOUT_MIN_3    = 5,
        CALL_TIMEOUT_MIN_4    = 6,
        CALL_TIMEOUT_MIN_5    = 7,
        CALL_TIMEOUT_MIN_6    = 8,
        CALL_TIMEOUT_MIN_8    = 9,
        CALL_TIMEOUT_MIN_10   = 10,
        CALL_TIMEOUT_MIN_12   = 11,
        CALL_TIMEOUT_MIN_15   = 12,
        CALL_TIMEOUT_MIN_20   = 13,
        CALL_TIMEOUT_MIN_30   = 14,
        CALL_TIMEOUT_MAX      = 14,
        //reserved 15

        //Call Timeout Setup Phase
        CALL_TIMEOUT_SETUP_PREDEF = 0,
        CALL_TIMEOUT_SETUP_1S     = 1,
        CALL_TIMEOUT_SETUP_2S     = 2,
        CALL_TIMEOUT_SETUP_5S     = 3,
        CALL_TIMEOUT_SETUP_10S    = 4,
        CALL_TIMEOUT_SETUP_20S    = 5,
        CALL_TIMEOUT_SETUP_30S    = 6,
        CALL_TIMEOUT_SETUP_60S    = 7,

        //Called Party Gateway SSI
        //Calling Party Gateway SSI
        //Forward Address Gateway SSI
        //Forwarded to User Gateway SSI
        //Included Party Gateway SSI
        //Served User Gateway SSI
        //Transmitting Party Gateway SSI
        GATEWAY_PABX_MIN = 16776960,
        GATEWAY_PABX_MAX = 16777183,
        GATEWAY_PSTN     = 16777184,

        //Circuit Mode Type
        CM_TYPE_SPEECH            = 0,
        CM_TYPE_UNPROTECTED       = 1,
        CM_TYPE_LOW_PROTECTION_1  = 2,
        CM_TYPE_LOW_PROTECTION_4  = 3,
        CM_TYPE_LOW_PROTECTION_8  = 4,
        CM_TYPE_HIGH_PROTECTION_1 = 5,
        CM_TYPE_HIGH_PROTECTION_4 = 6,
        CM_TYPE_HIGH_PROTECTION_8 = 7,

        //Class Of Usage
        COU_COU_1                   = 0,
        COU_NOT_SCANNED             = 1,
        COU_SCANNED_LOW_PRIORITY    = 2,
        COU_SCANNED_NORMAL_PRIORITY = 3,
        COU_SCANNED_SELECTED_GRP    = 4,
        COU_SCANNED_HIGH_PRIORITY   = 5,
        COU_COU_7                   = 6,
        COU_COU_8                   = 7,

        //CLIR Control
        CLIR_NOT_IMPLEMENTED = 0,
        //reserved 1
        CLIR_NOT_RESTRICTED  = 2,
        CLIR_RESTRICTED      = 3,

        //Communication Type
        COMM_TYPE_POINT_TO_POINT          = 0,
        COMM_TYPE_POINT_TO_MULTIPOINT     = 1,
        COMM_TYPE_POINT_TO_MULTIPOINT_ACK = 2, //reserved in MCIP
        COMM_TYPE_BROADCAST               = 3,
        COMM_TYPE_AMBIENCE_LISTENING      = 4,

        //Configured Call Forwarding Type
        CFG_CF_UNCONDITIONAL = 0,
        CFG_CF_BUSY          = 1,
        CFG_CF_NO_REPLY      = 2,
        CFG_CF_NOT_REACHABLE = 3,

        //Container User Type Id
        //Subs Container Type
        //Subs Content Type
        //User Type Id
        SUBS_TYPE_UNKNOWN    = 0,
        SUBS_TYPE_ROOT       = 1,
        SUBS_TYPE_VPN        = 2,
        SUBS_TYPE_DIVISION   = 3,
        SUBS_TYPE_FLEET      = 4,
        SUBS_TYPE_GRP        = 5,
        SUBS_TYPE_DGNA_IND   = 6,
        SUBS_TYPE_DGNA_GRP   = 7,
        SUBS_TYPE_ISSI       = 8,
        SUBS_TYPE_PHONE_EXTN = 9,
        //reserved 10-20
        SUBS_TYPE_DISPATCHER = 31,
        SUBS_TYPE_MOBILE     = 32,

        //Controlling Entity
        CTRL_ENTITY_MCS   = 0, //default if not present
        CTRL_ENTITY_TETRA = 1,

        //Delivery Report Request
        DEL_RPT_REQ_NONE          = 0,
        DEL_RPT_REQ_RCVD          = 1,
        DEL_RPT_REQ_CONSUMED      = 2,
        DEL_RPT_REQ_RCVD_CONSUMED = 3,

        //Delivery Status
        DEL_STAT_MSG_RCVD                       = 0,
        DEL_STAT_OK                             = 0, //MCIP
        //MCIP reserved 1-31
        DEL_STAT_SDS_RECEIPT_RPT_ACK            = 1,
        DEL_STAT_SDS_CONSUMED                   = 2,
        DEL_STAT_SDS_CONSUMED_RPT_ACK           = 3,
        DEL_STAT_SDS_FWD_EXT_NETWORK            = 4,
        DEL_STAT_SENT_TO_GRP                    = 5,
        DEL_STAT_CONCATENATION_PART_ACKD        = 6,
        DEL_STAT_CONGESTION_MSG_STORED          = 32,
        DEL_STAT_TMP_ERROR                      = 32, //MCIP
        //MCIP reserved 33-53
        DEL_STAT_MSG_STORED                     = 33,
        DEL_STAT_DEST_NOT_REACHABLE_MSG_STORED  = 34,
        DEL_STAT_EMPTY_GRP                      = 54,
        //MCIP reserved 55-63
        DEL_STAT_NETWORK_OVERLOAD               = 64,
        DEL_STAT_SVC_PERMANENTLY_UNAVAILABLE    = 65,
        DEL_STAT_SVC_TEMP_UNAVAILABLE           = 66,
        DEL_STAT_SRC_NOT_AUTHORIZED             = 67,
        DEL_STAT_DEST_NOT_AUTHORIZED            = 68,
        DEL_STAT_UNKNOWN_DEST                   = 69,
        DEL_STAT_UNKNOWN_FWD_ADDR               = 70,
        DEL_STAT_GRP_ADDR_INDV_SVC              = 71,
        //MCIP reserved 72-73
        DEL_STAT_VALIDITY_EXPIRED_NOT_RCVD      = 72,
        DEL_STAT_VALIDITY_EXPIRED_NOT_CONSUMED  = 73,
        DEL_STAT_ERROR_TX_PROBLEM               = 74,
        DEL_STAT_DEST_NOT_REGISTERED            = 75,
        //MCIP reserved 76-79
        DEL_STAT_DEST_Q_FULL                    = 76,
        DEL_STAT_MSG_TOO_LONG                   = 77,
        DEL_STAT_DEST_NOT_SUPPORT_SDS_TL        = 78,
        DEL_STAT_DEST_HOST_NOT_CONNECTED        = 79,
        DEL_STAT_PROTOCOL_NOT_SUPPORTED         = 80,
        //MCIP reserved 81-85
        DEL_STAT_CODING_SCHEME_NOT_SUPPORTED    = 81,
        DEL_STAT_DEST_MEMORY_FULL_MSG_DISCARDED = 82,
        DEL_STAT_DEST_NOT_ACCEPT_SDS            = 83,
        DEL_STAT_CONCATENATED_MSG_TOO_LONG      = 84,
        DEL_STAT_DEST_ADDR_PROHIBITED           = 86,
        //MCIP reserved 87-127
        DEL_STAT_CANNOT_ROUTE_EXT_NETWORK       = 87,
        DEL_STAT_UNKNOWN_EXT_SUBSCR             = 88,
        DEL_STAT_DEST_NOT_REACHABLE_DEL_FAILED  = 90,
        DEL_STAT_RECEPTION_LOW_LEVEL_ACK        = 91,
        DEL_STAT_CORRUPT_FIELD                  = 92,
        DEL_STAT_NOT_RCVD_CONCATENATION_PARTS   = 93,
        DEL_STAT_DEST_MEMORY_FULL               = 96,
        DEL_STAT_DEST_MEMORY_AVAILABLE          = 97,
        DEL_STAT_NO_PENDING_MSGS                = 99,
        DEL_STAT_FORMAT_ERROR                   = 128,
        DEL_STAT_GENERAL_ERROR                  = 129,
        DEL_STAT_INVALID_CALLED_PARTY           = 130,
        //available for user definition 131-159
        //reserved 160-258
        DEL_STAT_TEXT_DISTRIBUTION_ERROR        = 256,
        DEL_STAT_STOP_SENDING                   = 257,
        DEL_STAT_START_SENDING                  = 258,
        DEL_STAT_USER_DATA_LENGTH_ERROR         = 259,

        //DGNA Group Type
        DGNA_GT_INDIVIDUAL = 0,
        DGNA_GT_GRP        = 1,

        //Disconnect Cause
        DC_NOT_DEFINED                           = 0,
        DC_USER_REQUESTED                        = 1,
        DC_CALLED_PARTY_BUSY                     = 2,
        DC_CALLED_PARTY_NOT_REACHABLE            = 3,
        DC_CALLED_PARTY_NOT_SUPPORT_ENCR         = 4,
        DC_CONGESTION_IN_INFRASTRUCTURE          = 5,
        DC_NOT_ALLOWED_TRAFFIC                   = 6,
        DC_INCOMPATIBLE_TRAFFIC                  = 7,
        DC_SVC_NOT_AVAILABLE                     = 8,
        DC_PRE_EMPTIVE_USE_OF_RESOURCE           = 9,
        DC_INVALID_CALL_ID                       = 10,
        DC_CALLED_PARTY_REJECTION                = 11,
        DC_NO_CC_ENTITY                          = 12,
        DC_TIMER_EXPIRY                          = 13,
        DC_SWMI_DISCONNECT                       = 14,
        DC_NO_ACKNOWLEDGEMENT                    = 15,
        DC_UNKNOWN_TETRA_IDENTITY                = 16,
        DC_SS_SPECIFIC_DISCONNECTION             = 17,
        DC_UNKNOWN_EXT_SUBSCR_IDENTITY           = 18,
        DC_CALL_REST_OF_OTHER_USER_FAILED        = 19,
        DC_CALLED_PARTY_REQUIRE_ENCR             = 20,
        DC_CONCURRENT_SETUP_NOT_SUPPORTED        = 21,
        DC_CALLED_PARTY_UNDER_SAME_DM_GATE       = 22,
        //MCIP reserved 23-32
        DC_NON_OWNER_REQ                         = 23,
        DC_REPEATED_REQ                          = 32,
        DC_SPEECH_INACTIVITY_TIMEOUT             = 33,
        //MCIP reserved 34
        DC_SBS_ERROR                             = 34,
        DC_ALREADY_IN_TRAFFIC                    = 35,
        DC_END_OF_TRAFFIC                        = 35, //MCIP
        DC_TIMEOUT_IN_Q_CALLED_PARTY_BUSY        = 36,
        DC_TIMEOUT_DISCONNECTION                 = 36, //MCIP
        //MCIP reserved 37-114
        DC_TIMEOUT_IN_Q_WAIT_RESOURCE            = 37,
        DC_TIMEOUT_IN_Q_WAIT_CONFLICT_RESOLUTION = 38,
        DC_TIMEOUT_IN_Q_WAIT_CALLED_PARTY_RESP   = 39,
        DC_INVALID_CALLED_PARTY                  = 40,
        DC_NOT_ALLOWED_CALLED_PARTY              = 41,
        DC_INVALID_CALLING_PARTY                 = 42,
        DC_NOT_ALLOWED_CALLING_PARTY             = 43,
        DC_PREEMPTION                            = 44,
        DC_CALLING_PARTY_NOT_MEMBER              = 45,
        DC_MIGRATION_NOT_SUPPORTED               = 46,
        DC_CALL_NOT_RESTORED_AFTER_CELL_RESELECT = 47,
        DC_SENT                                  = 48,
        DC_DELIVERED                             = 49,
        DC_SENT_NOT_ACKD                         = 50,
        DC_CALLED_PARTY_OUTSIDE_VPN              = 51,
        DC_SVC_TEMP_NOT_AVAILABLE                = 52,
        DC_EMPTY_GRP                             = 54,
        DC_SBS_LINK_LOST                         = 55,
        DC_CALLED_PARTY_NOT_REGISTERED           = 56,
        DC_CALLED_PARTY_NOT_REACHED              = 57,
        DC_CALL_REST_FAILED_MS_ROAMED_OUT        = 58,
        DC_CALL_REST_FAILED_NO_RESOURCE          = 59,
        DC_END_OF_TRAFFIC_IN_LINK_SITE           = 115,
        DC_SETUP_INCONSISTENT_INFO               = 115, //MCIP
        DC_TIMEOUT_CALL_DISCONNECTION            = 116,
        //MCIP reserved 117-147
        DC_MOBILE_CALLED_PARTY                   = 900, //special internal value

        //Diverted Basic Service Information
        DBSI_SPEECH                       = 1,
        DBSI_CIRCUIT_MODE_DATA            = 2,
        DBSI_SPEECH_CIRCUIT_MODE_DATA     = 3,
        DBSI_SDS                          = 4,
        DBSI_SDS_SPEECH                   = 5,
        DBSI_SDS_CIRCUIT_MODE_DATA        = 6,
        DBSI_SDS_SPEECH_CIRCUIT_MODE_DATA = 7,

        //Encryption Control
        ENCR_CTRL_CLEAR = 0,
        ENCR_CTRL_E2EE  = 1,

        //Encryption Flag
        ENCR_FLAG_CLEAR    = 0,
        ENCR_FLAG_E2EE     = 1,
        ENCR_FLAG_AIE      = 2,
        ENCR_FLAG_E2EE_AIE = 3,

        //Fleet ID
        FLEET_ID_MAX = 1000,

        //Forward Address Type
        FWD_ADDR_TYPE_SNA  = 0,
        FWD_ADDR_TYPE_SSI  = 1,
        FWD_ADDR_TYPE_TSI  = 2,
        FWD_ADDR_TYPE_EXT  = 3,
        FWD_ADDR_TYPE_NONE = 7,

        //Group AI Encryption Mode
        GRP_AI_ENCR_MIXED     = 0,
        GRP_AI_ENCR_CLEAR     = 1,
        GRP_AI_ENCR_ENCRYPTED = 2,

        //Group Identity Attach Detach Mode
        GRP_ATT_DET_AMENDMENT  = 0,
        GRP_ATT_DET_DETACH_ALL = 1,

        //Group Identity Attach Detach Type Identifier
        GRP_ATT_DET_TYPE_ATTACH = 0,
        GRP_ATT_DET_TYPE_DETACH = 1,

        //Group Identity Attachment Mode
        GRP_ATTACH_YES_PERMANENT          = 0,
        GRP_ATTACH_YES_NXT_ISSIATTACH_YES = 1,
        GRP_ATTACH_YES_NXT_ISSIATTACH_NO  = 2,
        GRP_ATTACH_YES_NXT_LOCUPDATE_REQ  = 3,
        GRP_ATTACH_NO_ALLOWED_YES         = 4,
        GRP_ATTACH_NO_ALLOWED_NO          = 5,

        //Group Identity Detachment
        GRP_DETACH_UNKNOWN_GRP       = 0,
        GRP_DETACH_NO_VALID_ENCR_KEY = 1,
        GRP_DETACH_USER_INITIATED    = 2,
        GRP_DETACH_CAPACITY_EXCEEDED = 3,

        //Group Type
        GRP_TYPE_ALL      = 0,
        GRP_TYPE_STATIC   = 1,
        GRP_TYPE_DGNA_IND = 2,
        GRP_TYPE_DGNA_GRP = 3,
        //reserved 4-16

        //Hook Method Selection
        HOOK_NO  = 0,
        HOOK_YES = 1,

        //Idle Time
        IDLE_TIME_MIN_1   = 0,
        IDLE_TIME_MIN_15  = 1,
        IDLE_TIME_MIN_30  = 2,
        IDLE_TIME_HOUR_1  = 3,
        IDLE_TIME_HOUR_3  = 4,
        IDLE_TIME_HOUR_5  = 5,
        IDLE_TIME_HOUR_10 = 6,
        IDLE_TIME_HOUR_24 = 7,
        IDLE_TIME_HOUR_48 = 8,

        //Inclusion Type
        INCL_TYPE_CALL_OWNER_NO  = 0,
        INCL_TYPE_CALL_OWNER_YES = 1,

        //Invocation Fail Cause
        INVOKE_FAIL_OTHER                       = 0,
        INVOKE_FAIL_SS_NOT_SUBSCRIBED           = 1,
        INVOKE_FAIL_BASIC_CALL_FAILED           = 2,
        INVOKE_FAIL_MAX_INCL_REACHED            = 3,
        INVOKE_FAIL_REQD_SVC_NOT_AVAILABLE      = 4,
        INVOKE_FAIL_WRONG_CALL_ID               = 5,
        INVOKE_FAIL_CHANGE_OF_OWNER_NOT_ALLOWED = 6,

        //Invoked Call Forwarding Type
        INVOKED_CF_UNCONDITIONAL = 0,
        INVOKED_CF_BUSY          = 1,
        INVOKED_CF_NO_REPLY      = 2,
        INVOKED_CF_NOT_REACHABLE = 3,

        //ISSI Type
        ISSI_TYPE_TETRA = 1,
        ISSI_TYPE_MCS   = 2,
        ISSI_TYPE_DISP  = 3,

        //Line List
        LINE_LIST_PABX_MIN = 16776960,
        LINE_LIST_PABX_MAX = 16777183,
        LINE_LIST_PSTN     = 16777184,

        //Location Update Type
        LOC_UPDATE_TYPE_ROAMING             = 0,
        LOC_UPDATE_TYPE_MIGRATING           = 1,
        LOC_UPDATE_TYPE_PERIODIC            = 2,
        LOC_UPDATE_TYPE_REGISTER            = 3,
        LOC_UPDATE_TYPE_CALL_REST_ROAMING   = 4,
        LOC_UPDATE_TYPE_CALL_REST_MIGRATING = 5,
        LOC_UPDATE_TYPE_DEMAND              = 6,
        LOC_UPDATE_TYPE_DISABLED_MS         = 7,
        LOC_UPDATE_TYPE_DEREGISTER          = 11,

        //MC Subscribers Requested
        MC_SUBS_REQ_NO  = 0, //default if not present
        MC_SUBS_REQ_YES = 1,

        //Msg Num
        MSG_NUM_MIN = 1,
        MSG_NUM_MAX = 280,

        //Msg Ref
        MSG_REF_MIN = 0,
        MSG_REF_MAX = 255,

        //Msg Type (TETRA SDS)
        MSG_TYPE_SDS_TRANSFER = 0,
        MSG_TYPE_SDS_RPT      = 1,
        MSG_TYPE_SDS_ACK      = 2,

        //Notification Indicator
        NOTIF_IND_LE_BROAD        = 0,
        NOTIF_IND_LE_ACK          = 1,
        NOTIF_IND_LE_PAGE         = 2,
        NOTIF_IND_AL              = 3,
        NOTIF_IND_BARRED_SS_BIC   = 4,
        NOTIF_IND_BARRED_SS_BOC   = 5,
        NOTIF_IND_CALL_FWD        = 6,
        NOTIF_IND_FWD_ACT         = 7,
        NOTIF_IND_ID_PRES_REST    = 8,
        NOTIF_IND_PRES_REST_OVRRD = 9,
        NOTIF_IND_CALL_WAIT_INV   = 10,
        NOTIF_IND_CALL_HOLD       = 11,
        NOTIF_IND_CALL_HOLD_RETR  = 12,
        NOTIF_IND_INCL_CALL       = 13,
        NOTIF_IND_MULTIPARTY      = 14,
        NOTIF_IND_LSC_INV         = 15,
        NOTIF_IND_CALL_REJ_SS_AS  = 16,
        NOTIF_IND_SS_AS_NO        = 17,
        NOTIF_IND_ALERTED         = 18,
        NOTIF_IND_CONNECTED       = 19,
        NOTIF_IND_PROCEEDING      = 20,
        NOTIF_IND_SS_CFU          = 21,
        NOTIF_IND_SS_CFB          = 22,
        NOTIF_IND_SS_CFNRY        = 23,
        NOTIF_IND_SS_CFNRC        = 24,
        NOTIF_IND_AL_OR_SPEECH    = 25,
        NOTIF_IND_IMMINENT_DISC   = 26,
        NOTIF_IND_LIMITED_GRP_COV = 27,
        //reserved 28-63

        //Party Selection
        PARTY_SELECT_SIMPLEX = 0,
        PARTY_SELECT_CALLING = 1,
        PARTY_SELECT_CALLED  = 2,

        //Protocol Id
        PROTOCOL_OTAK                      = 1,
        PROTOCOL_SIMPLE_TEXT_MSG           = 2,
        PROTOCOL_SIMPLE_GPS                = 3,
        PROTOCOL_WDP_WAP                   = 4,
        PROTOCOL_WCMP                      = 5,
        PROTOCOL_M_DMO                     = 6,
        PROTOCOL_PIN_AUTH                  = 7,
        PROTOCOL_E2EE_MSG                  = 8,
        PROTOCOL_SIMPLE_IMMEDIATE_TEXT_MSG = 9,
        PROTOCOL_LOC_INFO                  = 10,
        PROTOCOL_CONCATENATED_SDS_MSG      = 12,
        PROTOCOL_TEXT_MSG_SDS_TL           = 130,
        PROTOCOL_GPS_SDS_TL                = 131,
        PROTOCOL_WDP_WAP_TL                = 132,
        PROTOCOL_WCMP_TL                   = 133,
        PROTOCOL_M_DMO_TL                  = 134,
        PROTOCOL_E2EE_MSG_TL               = 136,
        PROTOCOL_IMMEDIATE_TEXT_MSG        = 137,
        PROTOCOL_MSG_WITH_USER_DATA_HDR    = 138,
        PROTOCOL_CONCATENATED_SDS_TL_MSG   = 140,
        PROTOCOL_SEPURA_STATUS             = 200,

        //Request To Tx
        REQUEST_TX_SELF  = 0,
        REQUEST_TX_OTHER = 1,

        //Service Selection Short Form Report
        SSSFR_UPLINK_INDV_DOWNLINK_SHORT      = 0,
        SSSFR_UPLINK_INDV_OR_GRP_DOWNLINK_STD = 1,

        //Short Data Type
        SDS_TYPE_1 = 0,
        SDS_TYPE_2 = 1,
        SDS_TYPE_3 = 2,
        SDS_TYPE_4 = 3,

        //Short Report Type
        SHORT_RPT_TYPE_NOT_SUPPORTED = 0,
        SHORT_RPT_TYPE_MEMORY_FULL   = 1,
        SHORT_RPT_TYPE_RCVD          = 2,
        SHORT_RPT_TYPE_CONSUMED      = 3,

        //Simplex Duplex
        SIMPLEX_DUPLEX_SIMPLEX = 0,
        SIMPLEX_DUPLEX_DUPLEX  = 1,

        //Slots Per Frame
        SLOTSPERFRAME_1 = 0,
        SLOTSPERFRAME_2 = 1,
        SLOTSPERFRAME_3 = 2,
        SLOTSPERFRAME_4 = 3,

        //Speech Service
        SPEECH_SVC_TETRA       = 0,
        //reserved 1-2
        SPEECH_SVC_PROPRIETARY = 3,

        //SSCAD Flags
        SSCAD_FLG_ENABLED        = 1,
        SSCAD_FLG_DEFAULT_ACCEPT = 2,
        SSCAD_FLG_TERMINAL       = 4,

        //SSCAD Interception Cause
        SSCAD_INTERCEPT_SVC  = 1,
        SSCAD_INTERCEPT_SRC  = 2,
        SSCAD_INTERCEPT_DEST = 3,

        //SSCAD Invoke Command
        SSCAD_INVOKE_CMD_REQ    = 1,
        SSCAD_INVOKE_CMD_CANCEL = 2,

        //SSCAD Restricted Direction
        SSCAD_RESTRICT_DIR_IN    = 1,
        SSCAD_RESTRICT_DIR_OUT   = 2,
        SSCAD_RESTRICT_DIR_INOUT = 3,

        //SSCAD Restricted Peer
        SSCAD_RESTRICT_PEER_ANY = 16777215,

        //SSCAD Restricted Peer Type
        SSCAD_RESTRICT_PEER_TYPE_FLEET         = 1,
        SSCAD_RESTRICT_PEER_TYPE_GSSI          = 2,
        SSCAD_RESTRICT_PEER_TYPE_PABX_GW_SSI   = 3,
        SSCAD_RESTRICT_PEER_TYPE_PSTN_GW_SSI   = 4,
        SSCAD_RESTRICT_PEER_TYPE_ISSI          = 5,
        SSCAD_RESTRICT_PEER_TYPE_DISPATCH_ISSI = 6,

        //SSCAD Restricted Service
        SSCAD_RESTRICT_SVC_VOICE          = 1,
        SSCAD_RESTRICT_SVC_CM_DATA        = 2,
        SSCAD_RESTRICT_SVC_VOICE_AND_DATA = 3,

        //SSCAD Restriction Define Cmd
        SSCAD_RESTRICT_DC_ADD    = 1,
        SSCAD_RESTRICT_DC_REMOVE = 2,

        //SSCAD Result Sequence Number
        SSCAD_RES_SEQ_NUM_LAST = 0,

        //SSCF Active
        SSCF_ACTIVE_REDIRECT_YES = 0,
        SSCF_ACTIVE_REDIRECT_NO  = 1,

        //SSCF Deactivation Type
        SSCF_DEACTIVATE_TYPE_ALL      = 0,
        SSCF_DEACTIVATE_TYPE_SPECIFIC = 1,

        //SSCF Query Type
        SSCF_QUERY_TYPE_ANY      = 0,
        SSCF_QUERY_TYPE_SPECIFIC = 1,

        //SSCF Rejection Cause
        SSCF_REJ_CAUSE_SVC_NOT_SUBSCRD          = 0,
        SSCF_REJ_CAUSE_NOT_AUTHORIZED           = 1,
        SSCF_REJ_CAUSE_INVALID_FWD_TO_NUM       = 2,
        SSCF_REJ_CAUSE_SPECIFIC_SVC_NOT_SUBSCRD = 3,
        SSCF_REJ_CAUSE_SVC_NUM_NOT_ALLOWED      = 4,
        SSCF_REJ_CAUSE_INVALID_SERVED_USER_NUM  = 5,
        SSCF_REJ_CAUSE_TEMP_UNAVAILABLE         = 6,
        SSCF_REJ_CAUSE_INCOMPATIBLE_SVC         = 253,
        SSCF_REJ_CAUSE_INCOMPATIBLE_SSCFU       = 254,
        SSCF_REJ_CAUSE_GENERAL_ERROR            = 255,

        //SSCF State
        SSCF_STATE_ENABLED  = 0,
        SSCF_STATE_DISABLED = 1,

        //Status-Code
        STATUS_CODE_EMERGENCY = 0,
        //reserved 1-32768
        //available for user definition 32769-61439
        //available for TETRA 61440-65535

        //Storage
        STORAGE_ALLOWED_NO  = 0,
        STORAGE_ALLOWED_YES = 1,

        //Store Flag
        STORE_YES = 1,

        //Subs Permission
        SUBS_PERMISSION_GRANTED = 0,
        SUBS_PERMISSION_DENIED  = 1,

        //Text Coding Scheme
        TEXT_CODING_7BIT                     = 0,
        TEXT_CODING_8859_1_LATIN1            = 1,
        TEXT_CODING_8859_2_LATIN2            = 2,
        TEXT_CODING_8859_3_LATIN3            = 3,
        TEXT_CODING_8859_4_LATIN4            = 4,
        TEXT_CODING_8859_5_LATIN_CYRILLIC    = 5,
        TEXT_CODING_8859_6_LATIN_ARABIC      = 6,
        TEXT_CODING_8859_7_LATIN_GREEK       = 7,
        TEXT_CODING_8859_8_LATIN_HEBREW      = 8,
        TEXT_CODING_8859_9_LATIN5            = 9,
        TEXT_CODING_8859_10_LATIN6           = 10,
        TEXT_CODING_8859_13_LATIN7           = 11,
        TEXT_CODING_8859_14_LATIN8           = 12,
        TEXT_CODING_8859_15_LATIN0           = 13,
        TEXT_CODING_PC_CODE_P437_US          = 14,
        TEXT_CODING_PC_CODE_P737_GREEK2      = 15,
        TEXT_CODING_PC_CODE_P850_LATIN1      = 16,
        TEXT_CODING_PC_CODE_P852_EAST_EUROPE = 17,
        TEXT_CODING_PC_CODE_P855_CYRILLIC1   = 18,
        TEXT_CODING_PC_CODE_P857_TURKISH     = 19,
        TEXT_CODING_PC_CODE_P860_PORTUGUESE  = 20,
        TEXT_CODING_PC_CODE_P861_ICELANDIC   = 21,
        TEXT_CODING_PC_CODE_P863_CANADIAN_FR = 22,
        TEXT_CODING_PC_CODE_P865_NORDIC      = 23,
        TEXT_CODING_PC_CODE_P866_RUSSIAN     = 24,
        TEXT_CODING_PC_CODE_P869_GREEK       = 25,
        TEXT_CODING_10646_1_UCS2             = 26,

        //Total Messages
        TOTAL_MSG_MIN = 1,
        TOTAL_MSG_MAX = 280,

        //Tx Demand Priority
        TX_DEMAND_PRIORITY_LOW        = 0,
        TX_DEMAND_PRIORITY_HIGH       = 1,
        TX_DEMAND_PRIORITY_PREEMPTIVE = 2,
        TX_DEMAND_PRIORITY_EMERGENCY  = 3,

        //Tx Grant
        TRANSMISSION_GRANTED            = 0,
        TRANSMISSION_NOT_GRANTED        = 1,
        TRANSMISSION_QUEUED             = 2,
        TRANSMISSION_GRANTED_TO_ANOTHER = 3,

        //Tx Request Permission
        TX_REQ_PERM_ALLOWED_YES = 0,
        TX_REQ_PERM_ALLOWED_NO  = 1,

        //User Data Length
        USER_DATA_LEN_MIN = 0,
        USER_DATA_LEN_MAX = 1128,

        //Validity Period
        VALIDITY_PERIOD_NONE     = 0,
        VALIDITY_PERIOD_INFINITE = 31,

        UNDEFINED = -99999
    };
};
#endif //MSGVALUEBASE_H
