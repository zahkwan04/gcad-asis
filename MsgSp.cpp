/**
 * SCAD/STM protocol message implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: MsgSp.cpp 1907 2025-02-27 08:51:25Z zulzaidi $
 * @author Mohd Rashid
 * @author Mohd Rozaimi
 */
#if defined(SERVERAPP) && defined(APP_STM)
#include <cmath>        //pow(), log()
#endif
#include <assert.h>
#include <stdlib.h>     //rand(), srand()
#include <time.h>

#ifdef MSG_AES
#include "Aes.h"
#endif
#include "PalLock.h"
#include "MsgSp.h"

using namespace std;

//static initializers
MsgSp::TypeMapT      MsgSp::sTypeMap(createTypeMap());
MsgSp::FieldNameMapT MsgSp::sFieldNameMap(createFieldNameMap());

MsgSp::MsgSp(int type) : mType(type)
{
    mTimestampStr = Utils::getTimestamp();
}

MsgSp::MsgSp(const MsgSp &src, bool keepTimestamp) :
mType(src.mType), mFields(src.mFields)
{
    if (keepTimestamp)
        mTimestampStr = src.mTimestampStr;
    else
        mTimestampStr = Utils::getTimestamp();
}

MsgSp &MsgSp::operator=(const MsgSp &src)
{
    mType         = src.mType;
    mTimestampStr = src.mTimestampStr;
    mFields       = src.mFields;
    return *this;
}

ostream &operator<<(ostream &os, const MsgSp &msg)
{
    return os << msg.toString();
}

bool MsgSp::isValid() const
{
    return (sTypeMap.count(mType) != 0);
}

bool MsgSp::hasField(int key) const
{
    return (mFields.count(key) != 0);
}

MsgSp &MsgSp::addField(int key, const string &value)
{
    mFields[key] = value;
    return *this;
}

MsgSp &MsgSp::addField(int key, int value)
{
    ostringstream os;
    os << value;
    mFields[key] = os.str();
    return *this;
}

MsgSp &MsgSp::addField(const pair<int, string> &keyAndValue)
{
    mFields[keyAndValue.first] = keyAndValue.second;
    return *this;
}

MsgSp &MsgSp::appendField(int key, const string &value, char delim)
{
    //e.g. append "1,0,3201235" to "0,1,3201234" with delim ';',
    //     => "0,1,3201234;1,0,3201235"
    if (mFields.count(key) == 0)
        mFields[key] = value;
    else
        mFields[key].append(1, delim).append(value);
    return *this;
}

MsgSp &MsgSp::removeField(int key)
{
    mFields.erase(key);
    return *this;
}

MsgSp &MsgSp::reset(int type)
{
    if (type >= 0)
        mType = type;
    mFields.clear();
    mTimestampStr = Utils::getTimestamp();
    return *this;
}

string MsgSp::getFieldString(int key) const
{
    auto it = mFields.find(key);
    if (it != mFields.end())
        return it->second;
    return "";
}

int MsgSp::getFieldInt(int key) const
{
    int retVal = Value::UNDEFINED;
    auto it = mFields.find(key);
    if (it != mFields.end())
    {
        istringstream is(it->second);
        is >> retVal;
    }
    return retVal;
}

bool MsgSp::getFieldVals(int key, NestedListT &values) const
{
    auto it = mFields.find(key);
    if (it == mFields.end())
        return false;
    vector<int>    subelements;
    vector<string> vals;
    Utils::fromString(it->second, vals, Value::GID_LIST_DELIMITER);
    for (const auto &s : vals)
    {
        Utils::fromString(s, subelements, Value::LIST_DELIMITER);
        values.push_back(subelements);
        subelements.clear();
    }
    return true;
}

string MsgSp::getTimestamp(bool inclMs) const
{
    if (!inclMs) //strip the milliseconds part ".123" at the end
        return mTimestampStr.substr(0, mTimestampStr.length() - 4);
    return mTimestampStr;
}

#define CASE(a, b) \
    case Value::a##_##b: \
        oss << " "#b; \
        break   //no semicolon

string MsgSp::getFieldValueString(int key, const string &value) const
{
    ostringstream oss;
    oss << getFieldName(key) << ' ';
    string valStr(value);
    if (valStr.empty())
    {
        valStr = getFieldString(key);
        if (valStr.empty())
            return oss.str();
    }
    oss << valStr;
    vector<int> vals;
    switch (key)
    {
        case Field::ACK_REQ_FR_AFFECTED_USER:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ACK_REQ_FAU, NO);
                CASE(ACK_REQ_FAU, YES);
                default:
                    break;
            }
            break;
        case Field::ACK_REQUIRED:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ACK_REQ, NO);
                CASE(ACK_REQ, YES);
                default:
                    break;
            }
            break;
        case Field::AFFECTED_USER_TYPE:
        case Field::CALLING_PARTY_TYPE:
        case Field::FWD_ADDR_TYPE:
        case Field::FWD_TO_USER_TYPE:
        case Field::INCL_PARTY_TYPE:
        case Field::INTR_PARTY_TYPE:
        case Field::RESTRICTED_USER_TYPE:
        case Field::SERVED_USER_TYPE:
        case Field::TX_PARTY_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(IDENTITY_TYPE, UNKNOWN);
                CASE(IDENTITY_TYPE, ISSI);
                CASE(IDENTITY_TYPE, GSSI);
                CASE(IDENTITY_TYPE, PSTN);
                CASE(IDENTITY_TYPE, EXTN);
                CASE(IDENTITY_TYPE, VPN);
                CASE(IDENTITY_TYPE, DIVISION);
                CASE(IDENTITY_TYPE, FLEET);
                CASE(IDENTITY_TYPE, DISPATCHER);
                CASE(IDENTITY_TYPE, MOBILE);
                CASE(IDENTITY_TYPE, SNA);
                CASE(IDENTITY_TYPE, TSI);
                default:
                    break;
            }
            break;
        case Field::AI_ENCR_CTRL:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(AI_ENCR_CTRL, CLEAR);
                CASE(AI_ENCR_CTRL, AIR);
                default:
                    break;
            }
            break;
        case Field::ASG_DEASSIGN_STATUS:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ASGD, ASSIGNED);
                CASE(ASGD, DEASSIGNED);
                CASE(ASGD, PENDING_ASSIGN);
                CASE(ASGD, PENDING_DEASSIGN);
                CASE(ASGD, NOT_SUPPORTED);
                default:
                    break;
            }
            break;
        case Field::ASG_DUR:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ASG_DUR, PREDEFINED);
                CASE(ASG_DUR, SEC_10);
                CASE(ASG_DUR, SEC_30);
                CASE(ASG_DUR, SEC_100);
                CASE(ASG_DUR, SEC_200);
                CASE(ASG_DUR, SEC_400);
                CASE(ASG_DUR, SEC_1000);
                CASE(ASG_DUR, SEC_2000);
                CASE(ASG_DUR, IDLE);
                default:
                    break;
            }
            break;
        case Field::ASG_REP_RATE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ASG_REP, NO);
                CASE(ASG_REP, LOW);
                CASE(ASG_REP, NORMAL);
                CASE(ASG_REP, HIGH);
                default:
                    break;
            }
            break;
        case Field::CALL_OWNERSHIP:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_OWNER, NO);
                CASE(CALL_OWNER, YES);
                default:
                    break;
            }
            break;
        case Field::CALL_PRIORITY:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_PRIORITY, UNDEFINED);
                CASE(CALL_PRIORITY, LOWEST);
                CASE(CALL_PRIORITY, PREEMPTIVE_1);
                CASE(CALL_PRIORITY, PREEMPTIVE_2);
                CASE(CALL_PRIORITY, PREEMPTIVE_3);
                CASE(CALL_PRIORITY, PREEMPTIVE_4_EMERGENCY);
                default:
                    break;
            }
            break;
        case Field::CALL_QUEUED:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_QUEUED, NO);
                CASE(CALL_QUEUED, YES);
                default:
                    break;
            }
            break;
        case Field::CALL_STATUS:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_STAT, PROCEEDING);
                CASE(CALL_STAT, QUEUED);
                CASE(CALL_STAT, SUBSCR_PAGED);
                CASE(CALL_STAT, CONTINUE);
                CASE(CALL_STAT, HANG_TIME_EXPIRED);
                default:
                    break;
            }
            break;
        case Field::CALL_TIMEOUT:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_TIMEOUT, INFINITE);
                CASE(CALL_TIMEOUT, SEC_30);
                CASE(CALL_TIMEOUT, SEC_45);
                CASE(CALL_TIMEOUT, SEC_60);
                CASE(CALL_TIMEOUT, MIN_2);
                CASE(CALL_TIMEOUT, MIN_3);
                CASE(CALL_TIMEOUT, MIN_4);
                CASE(CALL_TIMEOUT, MIN_5);
                CASE(CALL_TIMEOUT, MIN_6);
                CASE(CALL_TIMEOUT, MIN_8);
                CASE(CALL_TIMEOUT, MIN_10);
                CASE(CALL_TIMEOUT, MIN_12);
                CASE(CALL_TIMEOUT, MIN_15);
                CASE(CALL_TIMEOUT, MIN_20);
                CASE(CALL_TIMEOUT, MIN_30);
                default:
                    break;
            }
            break;
        case Field::CALL_TIMEOUT_SETUP:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CALL_TIMEOUT_SETUP, PREDEF);
                CASE(CALL_TIMEOUT_SETUP, 1S);
                CASE(CALL_TIMEOUT_SETUP, 2S);
                CASE(CALL_TIMEOUT_SETUP, 5S);
                CASE(CALL_TIMEOUT_SETUP, 10S);
                CASE(CALL_TIMEOUT_SETUP, 20S);
                CASE(CALL_TIMEOUT_SETUP, 30S);
                CASE(CALL_TIMEOUT_SETUP, 60S);
                default:
                    break;
            }
            break;
        case Field::CALLED_PARTY_TYPE:
            //this can have multiple values in outgoing SDS/Status
            Utils::fromString(valStr, vals, Value::LIST_DELIMITER);
            for (auto &i : vals)
            {
                switch (i)
                {
                    CASE(IDENTITY_TYPE, UNKNOWN);
                    CASE(IDENTITY_TYPE, ISSI);
                    CASE(IDENTITY_TYPE, GSSI);
                    CASE(IDENTITY_TYPE, PSTN);
                    CASE(IDENTITY_TYPE, EXTN);
                    CASE(IDENTITY_TYPE, VPN);
                    CASE(IDENTITY_TYPE, DIVISION);
                    CASE(IDENTITY_TYPE, FLEET);
                    CASE(IDENTITY_TYPE, DISPATCHER);
                    CASE(IDENTITY_TYPE, MOBILE);
                    default:
                        break;
                }
            }
            break;
        case Field::CIRCUIT_MODE_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CM_TYPE, SPEECH);
                CASE(CM_TYPE, UNPROTECTED);
                CASE(CM_TYPE, LOW_PROTECTION_1);
                CASE(CM_TYPE, LOW_PROTECTION_4);
                CASE(CM_TYPE, LOW_PROTECTION_8);
                CASE(CM_TYPE, HIGH_PROTECTION_1);
                CASE(CM_TYPE, HIGH_PROTECTION_4);
                CASE(CM_TYPE, HIGH_PROTECTION_8);
                default:
                    break;
            }
            break;
        case Field::CLASS_OF_USAGE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(COU, COU_1);
                CASE(COU, NOT_SCANNED);
                CASE(COU, SCANNED_LOW_PRIORITY);
                CASE(COU, SCANNED_NORMAL_PRIORITY);
                CASE(COU, SCANNED_SELECTED_GRP);
                CASE(COU, SCANNED_HIGH_PRIORITY);
                CASE(COU, COU_7);
                CASE(COU, COU_8);
                default:
                    break;
            }
            break;
        case Field::CLIR_CTRL:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CLIR, NOT_IMPLEMENTED);
                CASE(CLIR, NOT_RESTRICTED);
                CASE(CLIR, RESTRICTED);
                default:
                    break;
            }
            break;
        case Field::COMM_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(COMM_TYPE, POINT_TO_POINT);
                CASE(COMM_TYPE, POINT_TO_MULTIPOINT);
                CASE(COMM_TYPE, POINT_TO_MULTIPOINT_ACK);
                CASE(COMM_TYPE, BROADCAST);
                CASE(COMM_TYPE, AMBIENCE_LISTENING);
                default:
                    break;
            }
            break;
        case Field::CONFIGURED_CALL_FWD_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CFG_CF, UNCONDITIONAL);
                CASE(CFG_CF, BUSY);
                CASE(CFG_CF, NO_REPLY);
                CASE(CFG_CF, NOT_REACHABLE);
                default:
                    break;
            }
            break;
        case Field::CTRL_ENTITY:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(CTRL_ENTITY, MCS);
                CASE(CTRL_ENTITY, TETRA);
                default:
                    break;
            }
            break;
        case Field::DELIVERY_RPT_REQ:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(DEL_RPT_REQ, NONE);
                CASE(DEL_RPT_REQ, RCVD);
                CASE(DEL_RPT_REQ, CONSUMED);
                CASE(DEL_RPT_REQ, RCVD_CONSUMED);
                default:
                    break;
            }
            break;
        case Field::DELIVERY_STATUS:
            switch (Utils::fromString<int>(valStr))
            {
#if defined(APP_STM) || defined(MOBILE)
                CASE(DEL_STAT, OK);
#else
                CASE(DEL_STAT, MSG_RCVD);
#endif
                CASE(DEL_STAT, SDS_RECEIPT_RPT_ACK);
                CASE(DEL_STAT, SDS_CONSUMED);
                CASE(DEL_STAT, SDS_CONSUMED_RPT_ACK);
                CASE(DEL_STAT, SDS_FWD_EXT_NETWORK);
                CASE(DEL_STAT, SENT_TO_GRP);
                CASE(DEL_STAT, CONCATENATION_PART_ACKD);
                CASE(DEL_STAT, CONGESTION_MSG_STORED);
                CASE(DEL_STAT, MSG_STORED);
                CASE(DEL_STAT, DEST_NOT_REACHABLE_MSG_STORED);
                CASE(DEL_STAT, EMPTY_GRP);
                CASE(DEL_STAT, NETWORK_OVERLOAD);
                CASE(DEL_STAT, SVC_PERMANENTLY_UNAVAILABLE);
                CASE(DEL_STAT, SVC_TEMP_UNAVAILABLE);
                CASE(DEL_STAT, SRC_NOT_AUTHORIZED);
                CASE(DEL_STAT, DEST_NOT_AUTHORIZED);
                CASE(DEL_STAT, UNKNOWN_DEST);
                CASE(DEL_STAT, UNKNOWN_FWD_ADDR);
                CASE(DEL_STAT, GRP_ADDR_INDV_SVC);
                CASE(DEL_STAT, VALIDITY_EXPIRED_NOT_RCVD);
                CASE(DEL_STAT, VALIDITY_EXPIRED_NOT_CONSUMED);
                CASE(DEL_STAT, ERROR_TX_PROBLEM);
                CASE(DEL_STAT, DEST_NOT_REGISTERED);
                CASE(DEL_STAT, DEST_Q_FULL);
                CASE(DEL_STAT, MSG_TOO_LONG);
                CASE(DEL_STAT, DEST_NOT_SUPPORT_SDS_TL);
                CASE(DEL_STAT, DEST_HOST_NOT_CONNECTED);
                CASE(DEL_STAT, PROTOCOL_NOT_SUPPORTED);
                CASE(DEL_STAT, CODING_SCHEME_NOT_SUPPORTED);
                CASE(DEL_STAT, DEST_MEMORY_FULL_MSG_DISCARDED);
                CASE(DEL_STAT, DEST_NOT_ACCEPT_SDS);
                CASE(DEL_STAT, CONCATENATED_MSG_TOO_LONG);
                CASE(DEL_STAT, DEST_ADDR_PROHIBITED);
                CASE(DEL_STAT, CANNOT_ROUTE_EXT_NETWORK);
                CASE(DEL_STAT, UNKNOWN_EXT_SUBSCR);
                CASE(DEL_STAT, DEST_NOT_REACHABLE_DEL_FAILED);
                CASE(DEL_STAT, RECEPTION_LOW_LEVEL_ACK);
                CASE(DEL_STAT, CORRUPT_FIELD);
                CASE(DEL_STAT, NOT_RCVD_CONCATENATION_PARTS);
                CASE(DEL_STAT, DEST_MEMORY_FULL);
                CASE(DEL_STAT, DEST_MEMORY_AVAILABLE);
                CASE(DEL_STAT, NO_PENDING_MSGS);
                CASE(DEL_STAT, FORMAT_ERROR);
                CASE(DEL_STAT, GENERAL_ERROR);
                CASE(DEL_STAT, INVALID_CALLED_PARTY);
                CASE(DEL_STAT, TEXT_DISTRIBUTION_ERROR);
                CASE(DEL_STAT, STOP_SENDING);
                CASE(DEL_STAT, START_SENDING);
                CASE(DEL_STAT, USER_DATA_LENGTH_ERROR);
                default:
                    break;
            }
            break;
        case Field::DGNA_GRP_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(DGNA_GT, INDIVIDUAL);
                CASE(DGNA_GT, GRP);
                default:
                    break;
            }
            break;
        case Field::DISCONNECT_CAUSE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(DC, NOT_DEFINED);
                CASE(DC, USER_REQUESTED);
                CASE(DC, CALLED_PARTY_BUSY);
                CASE(DC, CALLED_PARTY_NOT_REACHABLE);
                CASE(DC, CALLED_PARTY_NOT_SUPPORT_ENCR);
                CASE(DC, CONGESTION_IN_INFRASTRUCTURE);
                CASE(DC, NOT_ALLOWED_TRAFFIC);
                CASE(DC, INCOMPATIBLE_TRAFFIC);
                CASE(DC, SVC_NOT_AVAILABLE);
                CASE(DC, PRE_EMPTIVE_USE_OF_RESOURCE);
                CASE(DC, INVALID_CALL_ID);
                CASE(DC, CALLED_PARTY_REJECTION);
                CASE(DC, NO_CC_ENTITY);
                CASE(DC, TIMER_EXPIRY);
                CASE(DC, SWMI_DISCONNECT);
                CASE(DC, NO_ACKNOWLEDGEMENT);
                CASE(DC, UNKNOWN_TETRA_IDENTITY);
                CASE(DC, SS_SPECIFIC_DISCONNECTION);
                CASE(DC, UNKNOWN_EXT_SUBSCR_IDENTITY);
                CASE(DC, CALL_REST_OF_OTHER_USER_FAILED);
                CASE(DC, CALLED_PARTY_REQUIRE_ENCR);
                CASE(DC, CONCURRENT_SETUP_NOT_SUPPORTED);
                CASE(DC, CALLED_PARTY_UNDER_SAME_DM_GATE);
                CASE(DC, REPEATED_REQ);
                CASE(DC, SPEECH_INACTIVITY_TIMEOUT);
                CASE(DC, SBS_ERROR);
                CASE(DC, ALREADY_IN_TRAFFIC);
                CASE(DC, TIMEOUT_IN_Q_CALLED_PARTY_BUSY);
                CASE(DC, TIMEOUT_IN_Q_WAIT_RESOURCE);
                CASE(DC, TIMEOUT_IN_Q_WAIT_CONFLICT_RESOLUTION);
                CASE(DC, TIMEOUT_IN_Q_WAIT_CALLED_PARTY_RESP);
                CASE(DC, INVALID_CALLED_PARTY);
                CASE(DC, NOT_ALLOWED_CALLED_PARTY);
                CASE(DC, INVALID_CALLING_PARTY);
                CASE(DC, NOT_ALLOWED_CALLING_PARTY);
                CASE(DC, PREEMPTION);
                CASE(DC, CALLING_PARTY_NOT_MEMBER);
                CASE(DC, MIGRATION_NOT_SUPPORTED);
                CASE(DC, CALL_NOT_RESTORED_AFTER_CELL_RESELECT);
                CASE(DC, SENT);
                CASE(DC, DELIVERED);
                CASE(DC, SENT_NOT_ACKD);
                CASE(DC, CALLED_PARTY_OUTSIDE_VPN);
                CASE(DC, SVC_TEMP_NOT_AVAILABLE);
                CASE(DC, EMPTY_GRP);
                CASE(DC, SBS_LINK_LOST);
                CASE(DC, CALLED_PARTY_NOT_REGISTERED);
                CASE(DC, CALLED_PARTY_NOT_REACHED);
                CASE(DC, CALL_REST_FAILED_MS_ROAMED_OUT);
                CASE(DC, CALL_REST_FAILED_NO_RESOURCE);
                CASE(DC, END_OF_TRAFFIC_IN_LINK_SITE);
                CASE(DC, TIMEOUT_CALL_DISCONNECTION);
                CASE(DC, CALL_COMPLETED_ELSEWHERE);
                CASE(DC, MOBILE_CALLED_PARTY);
                default:
                    break;
            }
            break;
        case Field::DIVERTED_BASIC_SVC_INFO:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(DBSI, SPEECH);
                CASE(DBSI, CIRCUIT_MODE_DATA);
                CASE(DBSI, SPEECH_CIRCUIT_MODE_DATA);
                CASE(DBSI, SDS);
                CASE(DBSI, SDS_SPEECH);
                CASE(DBSI, SDS_CIRCUIT_MODE_DATA);
                CASE(DBSI, SDS_SPEECH_CIRCUIT_MODE_DATA);
                default:
                    break;
            }
            break;
        case Field::ENCR_CTRL:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ENCR_CTRL, CLEAR);
                CASE(ENCR_CTRL, E2EE);
                default:
                    break;
            }
            break;
        case Field::ENCR_FLAG:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(ENCR_FLAG, CLEAR);
                CASE(ENCR_FLAG, E2EE);
                CASE(ENCR_FLAG, AIE);
                CASE(ENCR_FLAG, E2EE_AIE);
                default:
                    break;
            }
            break;
        case Field::GRP_AI_ENCR_MODE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(GRP_AI_ENCR, MIXED);
                CASE(GRP_AI_ENCR, CLEAR);
                CASE(GRP_AI_ENCR, ENCRYPTED);
                default:
                    break;
            }
            break;
        case Field::GRP_ATT_MODE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(GRP_ATTACH, YES_PERMANENT);
                CASE(GRP_ATTACH, YES_NXT_ISSIATTACH_YES);
                CASE(GRP_ATTACH, YES_NXT_ISSIATTACH_NO);
                CASE(GRP_ATTACH, YES_NXT_LOCUPDATE_REQ);
                CASE(GRP_ATTACH, NO_ALLOWED_YES);
                CASE(GRP_ATTACH, NO_ALLOWED_NO);
                default:
                    break;
            }
            break;
        case Field::GRP_ATTACH_DETACH_MODE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(GRP_ATT_DET, AMENDMENT);
                CASE(GRP_ATT_DET, DETACH_ALL);
                default:
                    break;
            }
            break;
        case Field::GRP_NAME:
#ifdef MOBILE
            //Android ostringstream can handle UTF8 string
            oss << " '" << Utils::fromHexString(valStr) << "'";
#else
            oss << " '" << Utils::toString(Utils::hex2Wstring(valStr)) << "'";
#endif
            break;
        case Field::GRP_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(GRP_TYPE, ALL);
                CASE(GRP_TYPE, STATIC);
                CASE(GRP_TYPE, DGNA_IND);
                CASE(GRP_TYPE, DGNA_GRP);
                default:
                    break;
            }
            break;
        case Field::HOOK_METHOD:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(HOOK, NO);
                CASE(HOOK, YES);
                default:
                    break;
            }
            break;
        case Field::IDLE_TIME:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(IDLE_TIME, MIN_1);
                CASE(IDLE_TIME, MIN_15);
                CASE(IDLE_TIME, MIN_30);
                CASE(IDLE_TIME, HOUR_1);
                CASE(IDLE_TIME, HOUR_3);
                CASE(IDLE_TIME, HOUR_5);
                CASE(IDLE_TIME, HOUR_10);
                CASE(IDLE_TIME, HOUR_24);
                CASE(IDLE_TIME, HOUR_48);
                default:
                    break;
            }
            break;
        case Field::INCL_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(INCL_TYPE, CALL_OWNER_NO);
                CASE(INCL_TYPE, CALL_OWNER_YES);
                default:
                    break;
            }
            break;
        case Field::INVALID_FIELD:
            oss << ' ' << getFieldName(Utils::fromString<int>(valStr));
            break;
        case Field::INVALID_MSG_TYPE:
            oss << ' ' << getTypeName(Utils::fromString<int>(valStr));
            break;
        case Field::INVOCATION_FAIL_CAUSE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(INVOKE_FAIL, OTHER);
                CASE(INVOKE_FAIL, SS_NOT_SUBSCRIBED);
                CASE(INVOKE_FAIL, BASIC_CALL_FAILED);
                CASE(INVOKE_FAIL, MAX_INCL_REACHED);
                CASE(INVOKE_FAIL, REQD_SVC_NOT_AVAILABLE);
                CASE(INVOKE_FAIL, WRONG_CALL_ID);
                CASE(INVOKE_FAIL, CHANGE_OF_OWNER_NOT_ALLOWED);
                default:
                    break;
            }
            break;
        case Field::INVOKED_CALL_FWD_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(INVOKED_CF, UNCONDITIONAL);
                CASE(INVOKED_CF, BUSY);
                CASE(INVOKED_CF, NO_REPLY);
                CASE(INVOKED_CF, NOT_REACHABLE);
                default:
                    break;
            }
            break;
        case Field::LOC_UPDATE_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(LOC_UPDATE_TYPE, ROAMING);
                CASE(LOC_UPDATE_TYPE, MIGRATING);
                CASE(LOC_UPDATE_TYPE, PERIODIC);
                CASE(LOC_UPDATE_TYPE, REGISTER);
                CASE(LOC_UPDATE_TYPE, CALL_REST_ROAMING);
                CASE(LOC_UPDATE_TYPE, CALL_REST_MIGRATING);
                CASE(LOC_UPDATE_TYPE, DEMAND);
                CASE(LOC_UPDATE_TYPE, DISABLED_MS);
                CASE(LOC_UPDATE_TYPE, DEREGISTER);
                default:
                    break;
            }
            break;
        case Field::LOCATION_VALID:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(LOCATION_VALID, NO);
                CASE(LOCATION_VALID, YES);
                default:
                    break;
            }
            break;
        case Field::LOCK_ACTION:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(LOCK_ACTION, LOCK);
                CASE(LOCK_ACTION, UNLOCK);
                default:
                    break;
            }
            break;
        case Field::MC_SUBS_REQ:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(MC_SUBS_REQ, NO);
                CASE(MC_SUBS_REQ, YES);
                default:
                    break;
            }
            break;
        case Field::NETWORK_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(NETWORK_TYPE, TETRA);
                CASE(NETWORK_TYPE, TETRA2);
                CASE(NETWORK_TYPE, TETRA3);
                default:
                    break;
            }
            break;
        case Field::NOTIF_IND:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(NOTIF_IND, LE_BROAD);
                CASE(NOTIF_IND, LE_ACK);
                CASE(NOTIF_IND, LE_PAGE);
                CASE(NOTIF_IND, AL);
                CASE(NOTIF_IND, BARRED_SS_BIC);
                CASE(NOTIF_IND, BARRED_SS_BOC);
                CASE(NOTIF_IND, CALL_FWD);
                CASE(NOTIF_IND, FWD_ACT);
                CASE(NOTIF_IND, ID_PRES_REST);
                CASE(NOTIF_IND, PRES_REST_OVRRD);
                CASE(NOTIF_IND, CALL_WAIT_INV);
                CASE(NOTIF_IND, CALL_HOLD);
                CASE(NOTIF_IND, CALL_HOLD_RETR);
                CASE(NOTIF_IND, INCL_CALL);
                CASE(NOTIF_IND, MULTIPARTY);
                CASE(NOTIF_IND, LSC_INV);
                CASE(NOTIF_IND, CALL_REJ_SS_AS);
                CASE(NOTIF_IND, SS_AS_NO);
                CASE(NOTIF_IND, ALERTED);
                CASE(NOTIF_IND, CONNECTED);
                CASE(NOTIF_IND, PROCEEDING);
                CASE(NOTIF_IND, SS_CFU);
                CASE(NOTIF_IND, SS_CFB);
                CASE(NOTIF_IND, SS_CFNRY);
                CASE(NOTIF_IND, SS_CFNRC);
                CASE(NOTIF_IND, AL_OR_SPEECH);
                CASE(NOTIF_IND, IMMINENT_DISC);
                CASE(NOTIF_IND, LIMITED_GRP_COV);
                default:
                    break;
            }
            break;
        case Field::PARTY_SELECT:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(PARTY_SELECT, SIMPLEX);
                CASE(PARTY_SELECT, CALLING);
                CASE(PARTY_SELECT, CALLED);
                default:
                    break;
            }
            break;
        case Field::PROTOCOL_ID:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(PROTOCOL, OTAK);
                CASE(PROTOCOL, SIMPLE_TEXT_MSG);
                CASE(PROTOCOL, SIMPLE_GPS);
                CASE(PROTOCOL, WDP_WAP);
                CASE(PROTOCOL, WCMP);
                CASE(PROTOCOL, M_DMO);
                CASE(PROTOCOL, PIN_AUTH);
                CASE(PROTOCOL, E2EE_MSG);
                CASE(PROTOCOL, SIMPLE_IMMEDIATE_TEXT_MSG);
                CASE(PROTOCOL, LOC_INFO);
                CASE(PROTOCOL, CONCATENATED_SDS_MSG);
                CASE(PROTOCOL, TEXT_MSG_SDS_TL);
                CASE(PROTOCOL, GPS_SDS_TL);
                CASE(PROTOCOL, WDP_WAP_TL);
                CASE(PROTOCOL, WCMP_TL);
                CASE(PROTOCOL, M_DMO_TL);
                CASE(PROTOCOL, E2EE_MSG_TL);
                CASE(PROTOCOL, IMMEDIATE_TEXT_MSG);
                CASE(PROTOCOL, MSG_WITH_USER_DATA_HDR);
                CASE(PROTOCOL, CONCATENATED_SDS_TL_MSG);
                default:
                    break;
            }
            break;
        case Field::REQ_TO_TX:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(REQUEST_TX, SELF);
                CASE(REQUEST_TX, OTHER);
                default:
                    break;
            }
            break;
        case Field::RESULT:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(RESULT, OK);
                CASE(RESULT, REJECTED);
                CASE(RESULT, REJECTED_SECURITY);
                CASE(RESULT, NOT_AUTHORIZED);
                CASE(RESULT, UNDEFINED_ERROR);
                CASE(RESULT, TEMP_NOT_AVAILABLE);
                CASE(RESULT, TIMEOUT);
                CASE(RESULT, INVALID_PASSWORD);
                CASE(RESULT, INVALID_USER);
                CASE(RESULT, ALREADY_LOGGED_IN);
                CASE(RESULT, TX_PROBLEM);
                CASE(RESULT, NETW_NOT_REGISTERED);
                CASE(RESULT, RETRY);
                CASE(RESULT, NO_ACTIVE_CALL);
                CASE(RESULT, NO_VOIP_CHAN);
                CASE(RESULT, CALL_HAS_ENDED);
                CASE(RESULT, DUPLICATE_REQ);
                CASE(RESULT, DB_LINK_ERROR);
                CASE(RESULT, LIC_EXPIRED);
                CASE(RESULT, LIC_MAX);
                CASE(RESULT, LIC_INVALID_MAC);
                CASE(RESULT, INVALID_CLIENT);
                CASE(RESULT, INVALID_CLIENT_TYPE);
                CASE(RESULT, INACTIVE_USER);
                CASE(RESULT, BUSY_USER);
                CASE(RESULT, SSI_ALREADY_ASSIGNED);
                CASE(RESULT, GSSI_NOT_ASSIGNED);
                CASE(RESULT, MAX_GROUP_EXCEEDED);
                CASE(RESULT, MNEMONIC_TOO_LONG);
                CASE(RESULT, ERROR_GETTING_INFO);
                CASE(RESULT, ERROR_GETTING_DGNA_GRP);
                CASE(RESULT, GRP_NOT_EXIST);
                CASE(RESULT, ISSI_NOT_REACHABLE);
                CASE(RESULT, INVALID_COU);
                CASE(RESULT, INVALID_VPN);
                CASE(RESULT, INVALID_GSSI);
                CASE(RESULT, INVALID_ISSI);
                CASE(RESULT, INVALID_CALL_ID);
                CASE(RESULT, OUT_OF_CONTEXT);
                CASE(RESULT, QUERY_TYPE_NOT_SUPPORTED);
                CASE(RESULT, SVR_SHUTTING_DOWN);
                CASE(RESULT, TIMEOUT_VOIP);
                CASE(RESULT, TYPE_NOT_SUPPORTED);
                CASE(RESULT, WRONG_DEST);
                CASE(RESULT, GRP_EMPTY);
                CASE(RESULT, DGNA_GRP_EXIST);
                CASE(RESULT, DGNA_GRP_EMPTY);
                CASE(RESULT, DGNA_GRP_NOT_EMPTY);
                CASE(RESULT, DGNA_NOT_ALLOWED);
                CASE(RESULT, DGNA_NO_ELEMENT);
                CASE(RESULT, DGNA_MAX_REACHED);
                CASE(RESULT, DGNA_REQ_FAILED);
                CASE(RESULT, DGNA_MS_SUPPORT_NO);
                CASE(RESULT, DGNA_LACK_INFO);
                CASE(RESULT, DGNA_INVALID_AFFECTED_USER_TYPE);
                CASE(RESULT, DGNA_INVALID_ASG_DUR);
                CASE(RESULT, DGNA_INVALID_ASG_REP_RATE);
                CASE(RESULT, DGNA_INVALID_GRP_TYPE);
                CASE(RESULT, DGNA_INVALID_IDLE_TIME);
                CASE(RESULT, DGNA_INVALID_LIFETIME_DEFAULT);
                CASE(RESULT, DGNA_INVALID_MAX_CALL_PRIORITY);
                CASE(RESULT, DGNA_INVALID_MIN_CALL_PRIORITY);
                CASE(RESULT, DGNA_MISSING_ASG_DUR);
                CASE(RESULT, DGNA_MISSING_ASG_REP_RATE);
                CASE(RESULT, DGNA_MISSING_IDLE_TIME);
                CASE(RESULT, DGNA_MISSING_LIFETIME_DEFAULT);
                CASE(RESULT, SS_INVALID_TYPE);
                CASE(RESULT, SS_SVC_NOT_SUBSCRIBED);
                CASE(RESULT, SS_NOT_AVAILABLE);
                CASE(RESULT, MON_INVALID_NETW);
                CASE(RESULT, MON_NETW_NOT_ENABLED);
                CASE(RESULT, GRP_MON_FAILED);
                CASE(RESULT, GRP_MON_INVALID_GSSI);
                CASE(RESULT, GRP_MON_NOT_ALLOWED);
                CASE(RESULT, GRP_MON_MAX_REACHED);
                CASE(RESULT, GRP_MON_NOT_SUPPORTED);
                CASE(RESULT, MMSERR_INVALID);
                CASE(RESULT, MMSERR_FILE);
                CASE(RESULT, MMSERR_CONNECT);
                CASE(RESULT, MMSERR_UPLOAD);
                CASE(RESULT, MMSERR_DOWNLOAD);
                CASE(RESULT, MMSERR_DOWNLOAD_PERM);
                default:
                    break;
            }
            break;
        case Field::SHORT_DATA_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SDS, TYPE_1);
                CASE(SDS, TYPE_2);
                CASE(SDS, TYPE_3);
                CASE(SDS, TYPE_4);
                default:
                    break;
            }
            break;
        case Field::SHORT_RPT_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SHORT_RPT_TYPE, NOT_SUPPORTED);
                CASE(SHORT_RPT_TYPE, MEMORY_FULL);
                CASE(SHORT_RPT_TYPE, RCVD);
                CASE(SHORT_RPT_TYPE, CONSUMED);
                default:
                    break;
            }
            break;
        case Field::SIMPLEX_DUPLEX:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SIMPLEX_DUPLEX, SIMPLEX);
                CASE(SIMPLEX_DUPLEX, DUPLEX);
                default:
                    break;
            }
            break;
        case Field::SLOTS_PER_FRAME:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SLOTSPERFRAME, 1);
                CASE(SLOTSPERFRAME, 2);
                CASE(SLOTSPERFRAME, 3);
                CASE(SLOTSPERFRAME, 4);
                default:
                    break;
            }
            break;
        case Field::SPEECH_SVC:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SPEECH_SVC, TETRA);
                CASE(SPEECH_SVC, PROPRIETARY);
                default:
                    break;
            }
            break;
        case Field::SSCAD_FLAGS:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_FLG, ENABLED);
                CASE(SSCAD_FLG, DEFAULT_ACCEPT);
                CASE(SSCAD_FLG, TERMINAL);
                case Value::SSCAD_FLG_ENABLED +
                     Value::SSCAD_FLG_DEFAULT_ACCEPT:
                    oss << " ENABLED+DEFAULT_ACCEPT";
                    break;
                case Value::SSCAD_FLG_ENABLED +
                     Value::SSCAD_FLG_TERMINAL:
                    oss << " ENABLED+TERMINAL";
                    break;
                case Value::SSCAD_FLG_ENABLED +
                     Value::SSCAD_FLG_DEFAULT_ACCEPT +
                     Value::SSCAD_FLG_TERMINAL:
                    oss << " ENABLED+DEFAULT_ACCEPT+TERMINAL";
                    break;
                case Value::SSCAD_FLG_DEFAULT_ACCEPT +
                     Value::SSCAD_FLG_TERMINAL:
                    oss << " DEFAULT_ACCEPT+TERMINAL";
                    break;
                default:
                    break;
            }
            break;
        case Field::SSCAD_INTERCEPT_CAUSE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_INTERCEPT, SVC);
                CASE(SSCAD_INTERCEPT, SRC);
                CASE(SSCAD_INTERCEPT, DEST);
                default:
                    break;
            }
            break;
        case Field::SSCAD_INVOKE_CMD:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_INVOKE_CMD, REQ);
                CASE(SSCAD_INVOKE_CMD, CANCEL);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESTRICTED_DIR:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RESTRICT_DIR, IN);
                CASE(SSCAD_RESTRICT_DIR, OUT);
                CASE(SSCAD_RESTRICT_DIR, INOUT);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESTRICTED_PEER:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RESTRICT_PEER, ANY);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESTRICTED_PEER_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RESTRICT_PEER_TYPE, FLEET);
                CASE(SSCAD_RESTRICT_PEER_TYPE, GSSI);
                CASE(SSCAD_RESTRICT_PEER_TYPE, PABX_GW_SSI);
                CASE(SSCAD_RESTRICT_PEER_TYPE, PSTN_GW_SSI);
                CASE(SSCAD_RESTRICT_PEER_TYPE, ISSI);
                CASE(SSCAD_RESTRICT_PEER_TYPE, DISPATCH_ISSI);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESTRICTED_SVC:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RESTRICT_SVC, VOICE);
                CASE(SSCAD_RESTRICT_SVC, CM_DATA);
                CASE(SSCAD_RESTRICT_SVC, VOICE_AND_DATA);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESTRICTION_DEFINE_CMD:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RESTRICT_DC, ADD);
                CASE(SSCAD_RESTRICT_DC, REMOVE);
                default:
                    break;
            }
            break;
        case Field::SSCAD_RESULT_SEQ_NUM:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCAD_RES_SEQ_NUM, LAST);
                default:
                    break;
            }
            break;
        case Field::SSCF_ACTIVE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCF_ACTIVE_REDIRECT, YES);
                CASE(SSCF_ACTIVE_REDIRECT, NO);
                default:
                    break;
            }
            break;
        case Field::SSCF_DEACTIVATION_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCF_DEACTIVATE_TYPE, ALL);
                CASE(SSCF_DEACTIVATE_TYPE, SPECIFIC);
                default:
                    break;
            }
            break;
        case Field::SSCF_QUERY_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCF_QUERY_TYPE, ANY);
                CASE(SSCF_QUERY_TYPE, SPECIFIC);
                default:
                    break;
            }
            break;
        case Field::SSCF_REJECT_CAUSE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCF_REJ_CAUSE, SVC_NOT_SUBSCRD);
                CASE(SSCF_REJ_CAUSE, NOT_AUTHORIZED);
                CASE(SSCF_REJ_CAUSE, INVALID_FWD_TO_NUM);
                CASE(SSCF_REJ_CAUSE, SPECIFIC_SVC_NOT_SUBSCRD);
                CASE(SSCF_REJ_CAUSE, SVC_NUM_NOT_ALLOWED);
                CASE(SSCF_REJ_CAUSE, INVALID_SERVED_USER_NUM);
                CASE(SSCF_REJ_CAUSE, TEMP_UNAVAILABLE);
                CASE(SSCF_REJ_CAUSE, INCOMPATIBLE_SVC);
                CASE(SSCF_REJ_CAUSE, INCOMPATIBLE_SSCFU);
                CASE(SSCF_REJ_CAUSE, GENERAL_ERROR);
                default:
                    break;
            }
            break;
        case Field::SSCF_STATE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSCF_STATE, ENABLED);
                CASE(SSCF_STATE, DISABLED);
                default:
                    break;
            }
            break;
        case Field::STATUS_CODE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(STATUS_CODE, EMERGENCY);
                default:
                    break;
            }
            break;
        case Field::STORAGE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(STORAGE_ALLOWED, NO);
                CASE(STORAGE_ALLOWED, YES);
                default:
                    break;
            }
            break;
        case Field::STORE_FLAG:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(STORE, YES);
                default:
                    break;
            }
            break;
        case Field::SUBS_CONTAINER_TYPE:
        case Field::SUBS_CONTENT_TYPE:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SUBS_TYPE, UNKNOWN);
                CASE(SUBS_TYPE, ROOT);
                CASE(SUBS_TYPE, VPN);
                CASE(SUBS_TYPE, DIVISION);
                CASE(SUBS_TYPE, FLEET);
                CASE(SUBS_TYPE, GRP);
                CASE(SUBS_TYPE, DGNA_IND);
                CASE(SUBS_TYPE, DGNA_GRP);
                CASE(SUBS_TYPE, ISSI);
                CASE(SUBS_TYPE, PHONE_EXTN);
                CASE(SUBS_TYPE, DISPATCHER);
                CASE(SUBS_TYPE, MOBILE);
                default:
                    break;
            }
            break;
        case Field::SUBS_PERMISSION:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SUBS_PERMISSION, GRANTED);
                CASE(SUBS_PERMISSION, DENIED);
                default:
                    break;
            }
            break;
        case Field::SVC_SELECT_SHORT_FORM_RPT:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(SSSFR, UPLINK_INDV_DOWNLINK_SHORT);
                CASE(SSSFR, UPLINK_INDV_OR_GRP_DOWNLINK_STD);
                default:
                    break;
            }
            break;
        case Field::TX_DEMAND_PRIORITY:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(TX_DEMAND_PRIORITY, LOW);
                CASE(TX_DEMAND_PRIORITY, HIGH);
                CASE(TX_DEMAND_PRIORITY, PREEMPTIVE);
                CASE(TX_DEMAND_PRIORITY, EMERGENCY);
                default:
                    break;
            }
            break;
        case Field::TX_GRANT:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(TRANSMISSION, GRANTED);
                CASE(TRANSMISSION, NOT_GRANTED);
                CASE(TRANSMISSION, QUEUED);
                CASE(TRANSMISSION, GRANTED_TO_ANOTHER);
                default:
                    break;
            }
            break;
        case Field::TX_REQ_PERMISSION:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(TX_REQ_PERM_ALLOWED, YES);
                CASE(TX_REQ_PERM_ALLOWED, NO);
                default:
                    break;
            }
            break;
#ifdef DEBUG
        case Field::USER_DATA:
            switch (getFieldInt(Field::PROTOCOL_ID))
            {
                case Value::PROTOCOL_IMMEDIATE_TEXT_MSG:
                case Value::PROTOCOL_SIMPLE_IMMEDIATE_TEXT_MSG:
                case Value::PROTOCOL_SIMPLE_TEXT_MSG:
                case Value::PROTOCOL_TEXT_MSG_SDS_TL:
                    oss << "\n  \"" << Utils::fromHexString(valStr.substr(2))
                        << "\"";
                    break;
                default:
                    break; //do nothing
            }
            break;
#endif
        case Field::VALIDITY_PERIOD:
            switch (Utils::fromString<int>(valStr))
            {
                CASE(VALIDITY_PERIOD, NONE);
                CASE(VALIDITY_PERIOD, INFINITE);
                default:
                    break;
            }
            break;
        default:
            break;
    } //switch (key)
    return oss.str();
}

string MsgSp::toString() const
{
    ostringstream os;
    os << sFieldNameMap[Field::TYPE] << ' ' << mType << ' ' << getName()
       << Value::ENDL;
    for (const auto &it : mFields)
    {
        os << getFieldValueString(it.first, it.second) << Value::ENDL;
    }
    return os.str();
}

string MsgSp::serialize(const string &key) const
{
    ostringstream os;
    os << Field::TYPE << ' ' << mType << Value::ENDL;
    for (const auto &it : mFields)
    {
        os << it.first << ' ' << it.second << Value::ENDL;
    }
    os << Value::ENDL;
    if (!key.empty())
        return crypt(os.str(), key, true);
    return os.str();
}

string MsgSp::sipSerialize() const
{
    ostringstream os;
    os << Field::TYPE << ' ' << mType;
    for (const auto &it : mFields)
    {
        os << Value::SIP_DELIMITER << it.first << ' ' << it.second;
    }
    return os.str();
}

char *MsgSp::getBytes(int &len, const string &key) const
{
    string msg(serialize(key));
    len = msg.length();
    if (len == 0)
        return 0;
    char *bytes = new char[len];
    msg.copy(bytes, len);
    return bytes;
}

const string &MsgSp::getName() const
{
    return getTypeName(mType);
}

string MsgSp::getNameAndId() const
{
    ostringstream os;
    os << getTypeName(mType) << ' ' << getMsgId();
    return os.str();
}

int MsgSp::setMsgId()
{
#ifdef _WIN32
    static PalLock::LockT idLock; //no init needed
#elif defined QT_CORE_LIB
    static PalLock::LockT idLock(QMutex::Recursive);
#else
    static PalLock::LockT idLock = PTHREAD_MUTEX_INITIALIZER;
#endif
    static int id = MsgSp::Value::MSG_ID_MIN;
    PalLock::take(&idLock);
    int i = id;
    if (++id > MsgSp::Value::MSG_ID_MAX)
        id = MsgSp::Value::MSG_ID_MIN;
    PalLock::release(&idLock);
    addField(Field::MSG_ID, i);
    return i;
}

int MsgSp::getMsgId() const
{
    int id = getFieldInt(Field::MSG_ID);
    if (id == Value::UNDEFINED)
        return 0;
    return id;
}

void MsgSp::setSdsText(const string &text, int coding)
{
    //first byte (2 hex chars) is coding type
    string userData(Utils::toHexString(coding, 2) + Utils::toHexString(text));
    addField(Field::USER_DATA, userData);
    //each character is a 4-bit hex
    addField(Field::USER_DATA_LENGTH, 4 * userData.length());
}

int MsgSp::getSdsText(string &text) const
{
    int coding = -1;
    switch (getFieldInt(Field::PROTOCOL_ID))
    {
        //field exists only in SDS type-4
        case Value::PROTOCOL_IMMEDIATE_TEXT_MSG:
        case Value::PROTOCOL_SIMPLE_IMMEDIATE_TEXT_MSG:
        case Value::PROTOCOL_SIMPLE_TEXT_MSG:
        case Value::PROTOCOL_TEXT_MSG_SDS_TL:
        {
            string s(getFieldString(Field::USER_DATA));
            //first byte (2 hex chars) is coding type
            if (s.size() > 2)
            {
                coding = Utils::fromHexString<int>(s, 2);
                text = Utils::fromHexString(s.substr(2));
            }
            else if (s.size() == 2)
            {
                //should not really happen, but just in case
                coding = Utils::fromHexString<int>(s, 2);
            }
            break;
        }
        case Value::PROTOCOL_SIMPLE_GPS:
        {
            if (hasField(Field::LOCATION_LAT) && hasField(Field::LOCATION_LONG))
            {
                coding = Value::UNDEFINED;
                text.assign(getFieldString(Field::LOCATION_LAT)).append(",")
                    .append(getFieldString(Field::LOCATION_LONG));
            }
            break;
        }
        case Value::UNDEFINED:
        {
            if (getFieldInt(Field::SHORT_DATA_TYPE) != Value::SDS_TYPE_4)
                text = getFieldString(Field::USER_DATA); //raw hex data
            break;
        }
        default:
        {
            break; //do nothing
        }
    }
    return coding;
}

void MsgSp::setUserText()
{
    addField(Field::USER_DATA,
             Utils::toHexString(getFieldString(Field::USER_DATA)));
}

string MsgSp::getUserText()
{
    return Utils::fromHexString(getFieldString(Field::USER_DATA));
}

#if defined(SERVERAPP) && defined(APP_STM)
bool MsgSp::getSdsUserData()
{
    if (mType != Type::SDS_TRANSFER ||
        getFieldInt(Field::SHORT_DATA_TYPE) != Value::SDS_TYPE_4)
        return true; //type not applicable
    string s(getFieldString(Field::USER_DEFINED_DATA_4));
    removeField(Field::USER_DEFINED_DATA_4);
    //reference ETSI EN 300 392-2 V2.5.2 29.3-29.5
    //8b protocol ID
    int val = Utils::fromHexString<int>(s, 2);
    s.erase(0, 2);
    addField(Field::PROTOCOL_ID, val);
    switch (val)
    {
        case MsgSp::Value::PROTOCOL_CONCATENATED_SDS_TL_MSG:
        case MsgSp::Value::PROTOCOL_IMMEDIATE_TEXT_MSG:
        case MsgSp::Value::PROTOCOL_MSG_WITH_USER_DATA_HDR:
        case MsgSp::Value::PROTOCOL_TEXT_MSG_SDS_TL:
            break; //to process here
        case MsgSp::Value::PROTOCOL_SIMPLE_IMMEDIATE_TEXT_MSG:
        case MsgSp::Value::PROTOCOL_SIMPLE_TEXT_MSG:
            //no other elements to process
            //fallthrough
        default:
            if (!s.empty())
                addField(Field::USER_DATA, s);
            return true;
    }
    int n = getFieldInt(Field::USER_DATA_LENGTH) - 8; //# bits minus protocol ID
    removeField(Field::USER_DATA_LENGTH);
    //# bits must fit inside # chars - final char may carry 1-4 data bits
    if ((n + 3)/4 != (int) s.size())
        return false;
    //4b message type - SDS-TRANSFER/REPORT
    val = Utils::fromHexString<int>(s, 1);
    s.erase(0, 1);
    //SDS-REPORT comes in as SDS-TRANSFER
    if (val == Value::MSG_TYPE_SDS_TRANSFER)
    {
        if (n < 16)
            return false;
        //2b delivery report request
        //1b svc selection / short form report
        //1b storage
        val = Utils::fromHexString<int>(s, 1);
        s.erase(0, 1);
        addField(Field::DELIVERY_RPT_REQ, val >> 2);
        addField(Field::SVC_SELECT_SHORT_FORM_RPT, (val >> 1) & 0x1);
        n = val & 0x1;
    }
    else
    {
        if (n < 24)
            return false;
        mType = Type::SDS_RPT;
        //1b ack required
        //2b reserved
        //1b storage
        val = Utils::fromHexString<int>(s, 1);
        s.erase(0, 1);
        addField(Field::ACK_REQUIRED, val >> 3);
        n = val & 0x1;
        //8b delivery status
        val = Utils::fromHexString<int>(s, 2);
        s.erase(0, 2);
        addField(Field::DELIVERY_STATUS, val);
    }
    addField(Field::STORAGE, n);
    //8b msg ref
    val = Utils::fromHexString<int>(s, 2);
    s.erase(0, 2);
    addField(Field::MSG_REF, val);
    if (n == Value::STORAGE_ALLOWED_YES)
    {
        //5b validity period
        //3b forward addr type
        val = Utils::fromHexString<int>(s, 2);
        s.erase(0, 2);
        addField(Field::VALIDITY_PERIOD, val >> 3);
        val = val & 0x7;
        addField(Field::FWD_ADDR_TYPE, val);
        switch (val)
        {
            case Value::FWD_ADDR_TYPE_EXT:
            case Value::FWD_ADDR_TYPE_TSI:
                //type not supported - should not occur, but handle here to
                //remove the bytes just in case
                //24b forward addr ext
                addField(Field::FWD_ADDR, Utils::fromHexString<int>(s, 6));
                s.erase(0, 6);
                //8b number of external subscriber number digits
                val = Utils::fromHexString<int>(s, 2);
                s.erase(0, 2);
                if (val != 0)
                {
                    //4b*(val) external subscriber number digits
                    //4b dummy digit if val is odd
                    if (val % 2 != 0)
                        ++val;
                    s.erase(0, val);
                }
                break;
            case Value::FWD_ADDR_TYPE_SNA:
                //8b forward addr SNA - unsupported, should not occur, but
                //remove the bytes just in case
                s.erase(0, 2);
                break;
            case Value::FWD_ADDR_TYPE_SSI:
                //24b forward addr SSI
                addField(Field::FWD_ADDR, Utils::fromHexString<int>(s, 6));
                s.erase(0, 6);
                break;
            default:
                break; //do nothing
        }
    }
    //s now left with user data, if any
    if (!s.empty())
        addField(Field::USER_DATA, s);
    return true;
}

int MsgSp::getSdsUserData(int ref, string &data) const
{
    switch (mType)
    {
        case Type::SDS_RPT:
            break;
        case Type::SDS_TRANSFER:
            if (getFieldInt(Field::SHORT_DATA_TYPE) == Value::SDS_TYPE_4)
                break;
            //fallthrough
        default:
            return 0; //invalid type
    }
    //8b protocol ID
    int val = getFieldInt(Field::PROTOCOL_ID);
    data.assign(Utils::toHexString(val, 2));
    if (mType == Type::SDS_RPT)
    {
        //4b message type - SDS-REPORT
        data.append(Utils::toHexString(Value::MSG_TYPE_SDS_RPT, 1));
        //1b ack required => always NO here
        //2b reserved
        //1b storage => always NO here
        val = (Value::ACK_REQ_NO << 3) + Value::STORAGE_ALLOWED_NO;
        data.append(Utils::toHexString(val, 1))
        //8b delivery status
            .append(Utils::toHexString(getFieldInt(Field::DELIVERY_STATUS), 2))
        //8b msg ref - ensure value within range
            .append(Utils::toHexString(ref & 0xFF, 2));
    }
    else if (val >= 128) //TL
    {
        //4b message type - SDS-TRANSFER
        data.append(Utils::toHexString(Value::MSG_TYPE_SDS_TRANSFER, 1));
        //2b delivery report request
        //1b svc selection / short form report => always 1 here
        //1b storage => always NO here
        val = (getFieldInt(Field::DELIVERY_RPT_REQ) << 2) + (1 << 1) +
              Value::STORAGE_ALLOWED_NO;
        data.append(Utils::toHexString(val, 1))
        //8b msg ref - ensure value within range
            .append(Utils::toHexString(ref & 0xFF, 2));
    }
    if (hasField(Field::USER_DATA)) //optional & not expected in SDS_RPT
        data.append(getFieldString(Field::USER_DATA));
    return (data.size() * 4); //# bits
}

void MsgSp::setSdsUserData()
{
    string s;
    int n = getSdsUserData(getFieldInt(Field::MSG_REF), s);
    if (n > 0)
    {
        addField(Field::USER_DEFINED_DATA_4, s);
        addField(Field::USER_DATA_LENGTH, n); //# bits
        //check field required by network
        if (!hasField(Field::ENCR_FLAG))
            addField(Field::ENCR_FLAG, Value::ENCR_FLAG_CLEAR);
        removeField(Field::DELIVERY_STATUS);
        removeField(Field::DELIVERY_RPT_REQ);
        removeField(Field::USER_DATA);
        removeField(Field::PROTOCOL_ID);
        removeField(Field::STORAGE);
    }
}

bool MsgSp::setSdsLocData()
{
    if (mType != Type::GPS_LOC)
    {
        assert("Bad message type in MsgSp::setSdsLocData" == 0);
        return false;
    }
    int i;
    long val;
    double x;
    double r; //for rounding
    //time elapsed
    time_t t = time(0) - Utils::getTimeVal(getFieldString(Field::LOCATION_TIME));
    //8b protocol ID will be added by getSdsUserData(int, string)
    string s;
    if (hasField(Field::LOCATION_ALTITUDE))
    {
        //reference ETSI TS 100 392-18-1 V1.7.1 6.2.2 LIP Long
        //up to 'user defined data' - 103 bits
        //2b PDU type = LIP extension  = 01
        //4b PDU type extension = long = 0011
        //2b time type = time elapsed  = 01
        s.append("4D");
        //next 64b in a long
        //2b time elapsed
        if (t < 5)
        {
            val = 0;
        }
        else
        {
            if (t < 5 * 60)
                val = 1;
            else if (t < 30 * 60)
                val = 2;
            else
                val = 3;
            val = val << 62;
        }
        //4b location shape = point with altitude = 0100
        val |= ((((long) 4) & 0xF) << 58);
        //25b longitude two's complement signed integer, resolution 360/(2^25)
        getFieldVal(Field::LOCATION_LONG, x);
        r = (x >= 0)? 0.5: -0.5;
        val |= ((((long) (((x * pow(2, 25))/360) + r)) & 0x1FFFFFF) << 33);
        //24b latitude two's complement signed integer, resolution 180/(2^24)
        getFieldVal(Field::LOCATION_LAT, x);
        r = (x >= 0)? 0.5: -0.5;
        val |= ((((long) (((x * pow(2, 24))/180) + r)) & 0xFFFFFF) << 9);
        //1b altitude type = WGS84 = 0
        //11b altitude
        i = getFieldInt(Field::LOCATION_ALTITUDE);
        if (i <= -200)
            i = 1;
        else if (i <= 1000)
            i += 201; //2->1201, step 1m from -199m
        else if (i <= 2450)
            i = (((i - 1002)/2) + 1202); //1202->1926, step 2m from 1002m
        else if (i <= 11450)
            i = (((i - 2525)/75) + 1927); //1927->2046, step 75m from 2525m
        else
            i = 2047; //11525m or more
        //take only most significant 8b, i.e. discard last 3b
        val |= ((((long) i) & 0x7FF) >> 3);
        s.append(Utils::valToHexString(val, 16));
        //last 32b (31b + 1b padding) in an int
        //start with least significant 3b of altitude, then
        //3b velocity type = horizontal with direction ext = 101
        i = (i << 29) | (5 << 26);
        //7b horizontal velocity
        getFieldVal(Field::LOCATION_VELOCITY, x);
        int v;
        if (x < 0)
            v = 127;
        else if (x <= 28)
            v = x + 0.5; //round
        else if (x > 1043)
            v = 126;
        else
            v = ((log(x/16.0))/log(1.038)) + 13; //inverse 16*pow(1.038, val-13)
        i |= (v << 19);
        //8b direction extended degrees clockwise from North in steps of 360/256
        if (hasField(Field::LOCATION_DIRECTION))
        {
            getFieldVal(Field::LOCATION_DIRECTION, x);
            i |= ((((int) ((x * 256/360) + 0.5)) & 0xFF) << 11);
            removeField(Field::LOCATION_DIRECTION);
        }
        //1b ack request = no = 0
        //1b type of additional data = reason for sending = 0
        //8b reason for sending = max report interval = 129 = 0x81
        //1b padding
        i |= 0x102;
        s.append(Utils::valToHexString(i, 8));
        removeField(Field::LOCATION_ALTITUDE);
    }
    else
    {
        //reference ETSI TS 100 392-18-1 V1.7.1 6.2.1 LIP Short
        //up to 'user defined data' - 76 bits
        //2b PDU type = LIP short = 00
        //2b time elapsed
        if (t < 5)
            s.append("0");
        else if (t < 5 * 60)
            s.append("1");
        else if (t < 30 * 60)
            s.append("2");
        else
            s.append("3");
        //64 bits from longitude until type of additional data - use a long
        //25b longitude two's complement signed integer, resolution 360/(2^25)
        getFieldVal(Field::LOCATION_LONG, x);
        r = (x >= 0)? 0.5: -0.5;
        val = (((long) (((x * pow(2, 25))/360) + r)) & 0x1FFFFFF) << 39;
        //24b latitude two's complement signed integer, resolution 180/(2^24)
        getFieldVal(Field::LOCATION_LAT, x);
        r = (x >= 0)? 0.5: -0.5;
        val |= ((((long) (((x * pow(2, 24))/180) + r)) & 0xFFFFFF) << 15);
        //3b positition error
        i = getFieldInt(Field::LOCATION_ACCURACY);
        if (i < 0 ||
            getFieldInt(Field::LOCATION_VALID) == Value::LOCATION_VALID_NO)
            i = 7;
        else if (i <= 20)
            i = 1;
        else if (i <= 200)
            i = 2;
        else if (i <= 2000)
            i = 3;
        else if (i <= 20000)
            i = 4;
        else if (i <= 200000)
            i = 5;
        else
            i = 6;
        val |= ((((long) i) & 0x3) << 12);
        //7b horizontal velocity
        getFieldVal(Field::LOCATION_VELOCITY, x);
        if (x < 0)
            i = 127;
        else if (x <= 28)
            i = x + 0.5; //round
        else if (x > 1043)
            i = 126;
        else
            i = ((log(x/16.0))/log(1.038)) + 13; //inverse 16*pow(1.038, val-13)
        val |= ((((long) i) & 0x7F) << 5);
        //4b direction degrees clockwise from North in steps of 22.5
        if (hasField(Field::LOCATION_DIRECTION))
        {
            getFieldVal(Field::LOCATION_DIRECTION, x);
            val |= ((((long) ((x/22.5) + 0.5)) & 0xF) << 1);
            removeField(Field::LOCATION_DIRECTION);
        }
        //1b type of additional data = reason for sending = 0
        s.append(Utils::valToHexString(val, 16))
         .append("81"); //8b reason for sending = max report interval = 129
    }
    removeField(Field::LOCATION_TIME);
    removeField(Field::LOCATION_LONG);
    removeField(Field::LOCATION_LAT);
    removeField(Field::LOCATION_ACCURACY);
    removeField(Field::LOCATION_VELOCITY);
    removeField(Field::LOCATION_VALID);
    removeField(Field::TIME);
    setType(Type::SDS_TRANSFER);
    addField(Field::COMM_TYPE, Value::COMM_TYPE_POINT_TO_POINT);
    addField(Field::SHORT_DATA_TYPE, Value::SDS_TYPE_4);
    addField(Field::USER_DATA, s);
    addField(Field::USER_DATA_LENGTH, s.size() * 4); //# bits
    addField(Field::ENCR_FLAG, Value::ENCR_FLAG_CLEAR);
    //fields needed by getSdsUserData(int, string)
    addField(Field::PROTOCOL_ID, Value::PROTOCOL_LOC_INFO);
    addField(Field::DELIVERY_RPT_REQ, Value::DEL_RPT_REQ_NONE);
    return true;
}
#endif //SERVERAPP && APP_STM

bool MsgSp::isFinalResponse() const
{
    switch (mType)
    {
        case Type::SSCAD_QUERY:
            if (getFieldInt(Field::SSCAD_RESULT_SEQ_NUM) !=
                Value::SSCAD_RES_SEQ_NUM_LAST)
                return false;
            break;
        case Type::SSCF_SERVED_USERS_LIST:
        case Type::SSDGNA_GRP_LIST:
        case Type::SSDGNA_GSSI_LIST:
        case Type::SSDGNA_ISSI_LIST:
            if (getFieldInt(Field::MSG_NUM) !=
                getFieldInt(Field::TOTAL_MESSAGES))
                return false;
            break;
        default:
            break; //do nothing
    }
    return true;
}

bool MsgSp::isResultSuccessful() const
{
    switch (getFieldInt(Field::RESULT))
    {
        case Value::UNDEFINED:
        case Value::RESULT_OK:
            return true;
        case Value::RESULT_TX_PROBLEM:
            //this result is considered successful for STATUS_RPT -
            //see explanation in isSdsSuccessful()
            //case Value::DEL_STAT_DEST_NOT_REACHABLE_DEL_FAILED
            return (mType == Type::STATUS_RPT);
        default:
            return false;
    }
}

bool MsgSp::isSdsSuccessful() const
{
    if (mType == Type::SDS_RPT)
    {
        switch (getFieldInt(Field::DELIVERY_STATUS))
        {
            case Value::DEL_STAT_MSG_RCVD:
            case Value::DEL_STAT_SDS_RECEIPT_RPT_ACK:
            case Value::DEL_STAT_SDS_CONSUMED:
            case Value::DEL_STAT_SDS_CONSUMED_RPT_ACK:
            case Value::DEL_STAT_SDS_FWD_EXT_NETWORK:
            case Value::DEL_STAT_SENT_TO_GRP:
            case Value::DEL_STAT_CONCATENATION_PART_ACKD:
            case Value::DEL_STAT_CONGESTION_MSG_STORED:
            case Value::DEL_STAT_MSG_STORED:
            case Value::DEL_STAT_DEST_NOT_REACHABLE_MSG_STORED:
            case Value::DEL_STAT_DEST_NOT_REACHABLE_DEL_FAILED:
            //Teltronic 21/10/2015: DEL_STAT_DEST_NOT_REACHABLE_DEL_FAILED
            //means the infrastructure has sent the SDS through the air
            //interface but has not received low level ACK from the radio -
            //it deems the message sent and will send TDP-MONITOR-DSDS
            case Value::DEL_STAT_RECEPTION_LOW_LEVEL_ACK:
                return true;
            default:
                break; //do nothing
        }
    }
    else if (mType == Type::SDS_SHORT_RPT)
    {
        switch (getFieldInt(Field::SHORT_RPT_TYPE))
        {
            case Value::SHORT_RPT_TYPE_RCVD:
            case Value::SHORT_RPT_TYPE_CONSUMED:
                return true;
            default:
                break; //do nothing
        }
    }
    return false;
}

bool MsgSp::hasMmsError() const
{
    return isMmsError(getFieldInt(Field::RESULT));
}

int MsgSp::getPriority() const
{
    if (mType == Type::STATUS)
    {
        if (getFieldInt(Field::STATUS_CODE) == Value::STATUS_CODE_EMERGENCY)
            return Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY;
    }
    else if (hasField(Field::CALL_PRIORITY))
    {
        return getFieldInt(Field::CALL_PRIORITY);
    }
    else if (hasField(Field::TX_DEMAND_PRIORITY))
    {
        return getFieldInt(Field::TX_DEMAND_PRIORITY);
    }
    return 0;
}

int MsgSp::comparePriority(const MsgSp &msg) const
{
    if (mType != Type::STATUS && msg.getType() != Type::STATUS &&
        (mType != msg.getType() ||
         (mType == Type::CALL_TX_DEMAND &&
          getFieldInt(MsgSp::Field::CALL_ID) !=
              msg.getFieldInt(MsgSp::Field::CALL_ID))))
        return 0;
    int p1 = getPriority();
    int p2 = msg.getPriority();
    if (p1 < p2)
        return -1;
    return (p1 == p2)? 0: 1;
}

const string &MsgSp::getTypeName(int type)
{
    if (sTypeMap.count(type) == 0)
        return sTypeMap[Type::UNDEFINED].name;
    return sTypeMap[type].name;
}

const string &MsgSp::getFieldName(int id)
{
    if (sFieldNameMap.count(id) == 0)
        return sFieldNameMap[Field::UNDEFINED];
    return sFieldNameMap[id];
}

string MsgSp::getKey(string base)
{
#ifdef MSG_AES
    Aes::validateKey(base);
#endif
    char chk = 0; //checksum
    for (const auto &c : base)
    {
        chk ^= c;
    }
    for (auto &c : base)
    {
       c += chk;
    }
    return base;
}

int MsgSp::getMsgLen(const string &str)
{
    if (str.size() < LEN_SIZE)
        return (1 << (8 * LEN_SIZE)); //1 beyond max possible len
    return ((str.at(0) & 0xFF) << 8) + (str.at(1) & 0xFF);
}

MsgSp *MsgSp::parse(const string &str, const string &key)
{
    if (str.empty())
    {
        assert("Bad param in MsgSp::parse" == 0);
        return 0;
    }

    MsgSp  *newMsg = new MsgSp();
    bool    isValid = false;
    int     field;
    string  line;
    string  value;
    istringstream is;
    if (!key.empty())
        is.str(crypt(str, key, false));
    else
        is.str(str);
    while (getline(is, line) && line.length() > 1)
    {
        istringstream iss(line);
        iss >> field;
        if (!iss)
            continue;
        if (field == Field::TYPE)
        {
            //reuse 'field' variable to get the field numeric value
            if (iss >> field)
            {
                newMsg->setType(field);
                isValid = true;
            }
        }
        else
        {
            getline(iss, value);
            (void) Utils::trim(value);
            newMsg->addField(field, value);
        }
    }
    if (!isValid)
    {
        delete newMsg;
        return 0;
    }
    return newMsg;
}

string MsgSp::hexScramble(const string &str, const string &key, char delta)
{
    if (str.empty())
        return str;
    if (key.empty())
        return Utils::toHexString(str);
    auto rit = key.rbegin();
    if (delta != 0)
    {
        do
        {
            delta = rand() + time(NULL);
        }
        while (delta == 0);
        rit += delta % key.size();
    }
    string opStr;
    for (const auto &c : str)
    {
        if (rit == key.rend())
            rit = key.rbegin();
        opStr += (c ^ *rit) + delta;
        ++rit;
    }
    if (delta != 0)
        opStr.insert(opStr.size() - 1, &delta, 1); //insert delta
    return Utils::toHexString(opStr);
}

string MsgSp::hexUnscramble(const string &str, const string &key, char delta)
{
    if (str.empty())
        return str;
    string output(Utils::fromHexString(str));
    if (!key.empty())
    {
        auto rit = key.rbegin();
        if (delta != 0)
        {
            size_t n = output.size();
            delta = output[n - 2];
            output.erase(n - 2, 1); //remove delta
            rit += delta % key.size();
        }
        for (auto &c : output)
        {
            if (rit == key.rend())
                rit = key.rbegin();
            c = (c - delta) ^ *rit;
            ++rit;
        }
    }
    return output;
}

string MsgSp::scramble(const string &str, bool forward)
{
    //input and output characters must be within this range
    static const int MIN   = 32;        //0x20 = space
    static const int RANGE = 127 - MIN; //max 126 = 0x7E = '~'
    int delta;
    string out(str);
    if (forward)
    {
        //generate delta within range, but avoid MIN, which causes identity
        //transform for [MIN-max]
        do
        {
            delta = (rand() % RANGE) + MIN;
        }
        while (delta == MIN);
        bool dlr = false;
        for (auto &c : out)
        {
            c = ((c + delta - (2 * MIN)) % RANGE) + MIN;
            //prevent "${" which FreeSWITCH rejects because it is a variable
            //identifier - replace '{' in such sequence with value outside range
            if (c == '$')
            {
                dlr = true;
            }
            else if (dlr)
            {
                if (c == '{')
                    c |= 0x80; //will be read as negative
                else
                    dlr = false;
            }
        }
        out.insert(1, 1, delta);
    }
    else
    {
        //reverse
        delta = out[1];
        out.erase(1, 1);
        for (auto &c : out)
        {
            if (c < 0)
                c &= 0x7F;
            c = ((c + RANGE - delta) % RANGE) + MIN;
            if (c == Value::SIP_DELIMITER)
                c = Value::ENDL;
        }
        //string for MsgSp::parse() must end with double ENDL
        out.append(1, Value::ENDL);
    }
    return out;
}

string MsgSp::crypt(const string &str, const string &key, bool encrypt)
{
    assert(!key.empty());
    char delta;
#ifdef MSG_AES
    size_t len;
    string outStr;
    string k(key);
    if (encrypt)
    {
        //set delta which must not be 0
        do
        {
            //on Windows client, for unknown reason, the sequence of rand()
            //values in the first 4 messages starting from LOGIN are always the
            //same (41, 35, 190, 132), and the rest are random, whether on login
            //after startup, or relogin after logout -
            //to prevent that, add another dynamic value
            delta = rand() + time(NULL);
        }
        while (delta == 0);
        //modify k with delta
        for (auto &c : k)
        {
            c += delta;
        }
        len = k.size();
        k.insert(0, k.substr(len - (delta % len))).erase(len); //rotate by delta
        outStr = Aes::encrypt(str, k);
        //insert pad size
        outStr.insert(1, 1, (outStr.size() - str.size()) & 0xFF);
        len = outStr.size();
        outStr.insert(len - 1, &delta, 1); //insert delta
        //start output with 2-byte length, enough for max message size
        ++len; //including delta
        outStr.insert(0, 1, (len >> 8) & 0xFF).insert(1, 1, len & 0xFF);
    }
    else
    {
        len = str.size();
        outStr = str;
        delta = outStr[len - 2]; //recover and remove delta
        outStr.erase(len - 2, 1);
        for (auto &c : k)
        {
            c += delta;
        }
        len = k.size();
        k.insert(0, k.substr(len - (delta % len))).erase(len);
        len = outStr[1] & 0xFF; //recover and remove pad size
        outStr.erase(1, 1);
        outStr = Aes::decrypt(outStr, k);
        if (len != 0 && len < outStr.size()) //2nd check is for error protection
            outStr.erase(outStr.size() - len); //remove pad
    }
#else //MSG_AES
    size_t len = str.size();
    char lastc = 0;
    string outStr(str);
    auto itk = key.begin();
    if (encrypt)
    {
        //set delta which must not be 0
        do
        {
            delta = rand() + time(NULL);
        }
        while (delta == 0);
        //modify every outStr character using key (starting at delta) and delta,
        //correlated with previous character
        itk += ((static_cast<int>(delta) & 0xFF) % key.size());
        for (auto &c : outStr)
        {
            if (itk == key.end())
                itk = key.begin();
            c = (c + *itk++ + delta) ^ lastc;
            lastc = c;
        }
        outStr.insert(len - 1, &delta, 1); //insert delta
        //start output with 2-byte length, enough for max message size
        ++len; //including delta
        outStr.insert(0, 1, (len >> 8) & 0xFF).insert(1, 1, len & 0xFF);
    } //if (encrypt)
    else
    {
        delta = outStr[len - 2];
        outStr.erase(len - 2, 1); //remove delta
        itk += ((static_cast<int>(delta) & 0xFF) % key.size());
        char tmp;
        for (auto &c : outStr)
        {
            if (itk == key.end())
                itk = key.begin();
            tmp = c;
            c = (c ^ lastc) - *itk++ - delta;
            lastc = tmp;
        }
    }
#endif //MSG_AES
    return outStr;
}

MsgSp::TypeMapT MsgSp::createTypeMap()
{
    TypeMapT m;
    m[Type::UNDEFINED] =
        MsgInfo("UNDEFINED",                  "1.0");
    m[Type::BRANCH_DATA] =
        MsgInfo("BRANCH-DATA",                "1.0");
    m[Type::CALL_ALERT] =
        MsgInfo("CALL-ALERT",                 "1.0");
    m[Type::CALL_CONNECT] =
        MsgInfo("CALL-CONNECT",               "1.0");
    m[Type::CALL_CONNECT_ACK] =
        MsgInfo("CALL-CONNECT-ACK",           "1.0");
    m[Type::CALL_DISCONNECT] =
        MsgInfo("CALL-DISCONNECT",            "1.0");
    m[Type::CALL_FINISH] =
        MsgInfo("CALL-FINISH",                "1.0");
    m[Type::CALL_INFO] =
        MsgInfo("CALL-INFO",                  "1.0");
    m[Type::CALL_PROCEEDING] =
        MsgInfo("CALL-PROCEEDING",            "1.0");
    m[Type::CALL_RELEASE] =
        MsgInfo("CALL-RELEASE",               "1.0");
    m[Type::CALL_SETUP] =
        MsgInfo("CALL-SETUP",                 "1.0");
    m[Type::CALL_SSCF_FWD] =
        MsgInfo("CALL-SSCF-FORWARDED",        "1.0");
    m[Type::CALL_TX_CEASED] =
        MsgInfo("CALL-TX-CEASED",             "1.0");
    m[Type::CALL_TX_DEMAND] =
        MsgInfo("CALL-TX-DEMAND",             "1.0");
    m[Type::CALL_TX_GRANTED] =
        MsgInfo("CALL-TX-GRANTED",            "1.0");
    m[Type::CALL_TX_INTERRUPT] =
        MsgInfo("CALL-TX-INTERRUPT",          "1.0");
    m[Type::CHANGE_PASSWORD] =
        MsgInfo("CHANGE-PASSWORD",            "1.0");
    m[Type::GPS_LOC] =
        MsgInfo("GPS-LOCATION",               "1.0");
    m[Type::GPS_MON_START] =
        MsgInfo("GPS-MON-START",              "1.0");
    m[Type::GPS_MON_STOP] =
        MsgInfo("GPS-MON-STOP",               "1.0");
    m[Type::INCIDENT_LOCK] =
        MsgInfo("INCIDENT-LOCK",              "1.0");
    m[Type::INCIDENT_UPDATE] =
        MsgInfo("INCIDENT-UPDATE",            "1.0");
    m[Type::LISTEN_CONNECT] =
        MsgInfo("LISTEN-CONNECT",             "1.0");
    m[Type::LISTEN_DISCONNECT] =
        MsgInfo("LISTEN-DISCONNECT",          "1.0");
    m[Type::LISTEN_RELEASE] =
        MsgInfo("LISTEN-RELEASE",             "1.0");
    m[Type::LOGIN] =
        MsgInfo("LOGIN",                      "1.0");
    m[Type::LOGOUT] =
        MsgInfo("LOGOUT",                     "1.0");
    m[Type::MMS_RPT] =
        MsgInfo("MMS-REPORT",                 "1.0");
    m[Type::MMS_TRANSFER] =
        MsgInfo("MMS-TRANSFER",               "1.0");
    m[Type::MON_CONNECT] =
        MsgInfo("MON-CONNECT",                "1.0");
    m[Type::MON_DISCONNECT] =
        MsgInfo("MON-DISCONNECT",             "1.0");
    m[Type::MON_GRP_ATTACH_DETACH] =
        MsgInfo("MON-GROUP-ATTACH-DETACH",    "1.0");
    m[Type::MON_INFO] =
        MsgInfo("MON-INFO",                   "1.0");
    m[Type::MON_LOC] =
        MsgInfo("MON-LOCATION",               "1.0");
    m[Type::MON_SDS] =
        MsgInfo("MON-SDS",                    "1.0");
    m[Type::MON_SETUP] =
        MsgInfo("MON-SETUP",                  "1.0");
    m[Type::MON_SSCF_FWD] =
        MsgInfo("MON-SSCF-FORWARDED",         "1.0");
    m[Type::MON_SSDGNA_ASSIGN] =
        MsgInfo("MON-SSDGNA-ASSIGN",          "1.0");
    m[Type::MON_SSDGNA_DEASSIGN] =
        MsgInfo("MON-SSDGNA-DEASSIGN",        "1.0");
    m[Type::MON_SSDGNA_DEFINE] =
        MsgInfo("MON-SSDGNA-DEFINE",          "1.0");
    m[Type::MON_SSDGNA_DELETE] =
        MsgInfo("MON-SSDGNA-DELETE",          "1.0");
    m[Type::MON_SSIC_INCL] =
        MsgInfo("MON-SSIC-INCLUDED",          "1.0");
    m[Type::MON_SSIC_RELEASE] =
        MsgInfo("MON-SSIC-RELEASE",           "1.0");
    m[Type::MON_START] =
        MsgInfo("MON-START",                  "1.0");
    m[Type::MON_STATUS] =
        MsgInfo("MON-STATUS",                 "1.0");
    m[Type::MON_STOP] =
        MsgInfo("MON-STOP",                   "1.0");
    m[Type::MON_SUBS_ASSIGN] =
        MsgInfo("MON-SUBS-ASSIGN",            "1.0");
    m[Type::MON_SUBS_DEASSIGN] =
        MsgInfo("MON-SUBS-DEASSIGN",          "1.0");
    m[Type::MON_SUBS_DEFINE] =
        MsgInfo("MON-SUBS-DEFINE",            "1.0");
    m[Type::MON_SUBS_DELETE] =
        MsgInfo("MON-SUBS-DELETE",            "1.0");
    m[Type::MON_SUBS_DESC] =
        MsgInfo("MON-SUBS-DESC",              "1.0");
    m[Type::MON_SUBS_PERMISSION] =
        MsgInfo("MON-SUBS-PERMISSION",        "1.0");
    m[Type::MON_TX_CEASED] =
        MsgInfo("MON-TX-CEASED",              "1.0");
    m[Type::MON_TX_GRANTED] =
        MsgInfo("MON-TX-GRANTED",             "1.0");
    m[Type::PASSWORD] =
        MsgInfo("PASSWORD",                   "1.0");
    m[Type::POI_DELETE] =
        MsgInfo("POI-DELETE",                 "1.0");
    m[Type::POI_UPDATE] =
        MsgInfo("POI-UPDATE",                 "1.0");
    m[Type::REMOTE_SERVER_DISCONNECTED] =
        MsgInfo("REMOTE-SERVER-DISCONNECTED", "1.0");
    m[Type::REMOTE_SERVER_MAX_ATTEMPTS] =
        MsgInfo("REMOTE-SERVER-MAX-ATTEMPTS", "1.0");
    m[Type::REMOTE_SERVER_TIMEOUT] =
        MsgInfo("REMOTE-SERVER-TIMEOUT",      "1.0");
    m[Type::SDS_ACK] =
        MsgInfo("SDS-ACK",                    "1.0");
    m[Type::SDS_RPT] =
        MsgInfo("SDS-REPORT",                 "1.0");
    m[Type::SDS_SHORT_RPT] =
        MsgInfo("SDS-SHORT-REPORT",           "1.0");
    m[Type::SDS_TRANSFER] =
        MsgInfo("SDS-TRANSFER",               "1.0");
    m[Type::SSCAD_CONFIG_DEFINE] =
        MsgInfo("SSCAD-CONFIG-DEFINE",        "1.0");
    m[Type::SSCAD_INVOKE] =
        MsgInfo("SSCAD-INVOKE",               "1.0");
    m[Type::SSCAD_QUERY] =
        MsgInfo("SSCAD-QUERY",                "1.0");
    m[Type::SSCAD_RESTRICTION_DEFINE] =
        MsgInfo("SSCAD-RESTRICTION-DEFINE",   "1.0");
    m[Type::SSCF_ACTIVATE] =
        MsgInfo("SSCF-ACTIVATE",              "1.0");
    m[Type::SSCF_DEACTIVATE] =
        MsgInfo("SSCF-DEACTIVATE",            "1.0");
    m[Type::SSCF_GENERAL_ACTIVATE] =
        MsgInfo("SSCF-GENERAL-ACTIVATE",      "1.0");
    m[Type::SSCF_GENERAL_QUERY] =
        MsgInfo("SSCF-GENERAL-QUERY",         "1.0");
    m[Type::SSCF_QUERY] =
        MsgInfo("SSCF-QUERY",                 "1.0");
    m[Type::SSCF_SERVED_USERS_LIST] =
        MsgInfo("SSCF-SERVED-USERS-LIST",     "1.0");
    m[Type::SSDGNA_ASSIGN] =
        MsgInfo("SSDGNA-ASSIGN",              "1.0");
    m[Type::SSDGNA_QUERY_GRP] =
        MsgInfo("SSDGNA-QUERY-GROUP",         "1.0");
    m[Type::SSDGNA_DEASSIGN] =
        MsgInfo("SSDGNA-DEASSIGN",            "1.0");
    m[Type::SSDGNA_DEFINE] =
        MsgInfo("SSDGNA_DEFINE",              "1.0");
    m[Type::SSDGNA_DELETE] =
        MsgInfo("SSDGNA-DELETE",              "1.0");
    m[Type::SSDGNA_GRP_LIST] =
        MsgInfo("SSDGNA-GROUP-LIST",          "1.0");
    m[Type::SSDGNA_GSSI_LIST] =
        MsgInfo("SSDGNA-GSSI-LIST",           "1.0");
    m[Type::SSDGNA_ISSI_LIST] =
        MsgInfo("SSDGNA-ISSI-LIST",           "1.0");
    m[Type::SSDGNA_RESTART_ASG] =
        MsgInfo("SSDGNA-RESTART-ASSIGNMENT",  "1.0");
    m[Type::SSIC_CANCEL] =
        MsgInfo("SSIC-CANCEL",                "1.0");
    m[Type::SSIC_DISCONNECT] =
        MsgInfo("SSIC-DISCONNECT",            "1.0");
    m[Type::SSIC_INCL] =
        MsgInfo("SSIC-INCLUDED",              "1.0");
    m[Type::SSIC_INVOCATION_FAILURE] =
        MsgInfo("SSIC-INVOCATION-FAILURE",    "1.0");
    m[Type::SSIC_INVOKE] =
        MsgInfo("SSIC-INVOKE",                "1.0");
    m[Type::SSIC_RELEASE] =
        MsgInfo("SSIC-RELEASE",               "1.0");
    m[Type::STATUS] =
        MsgInfo("STATUS",                     "1.0");
    m[Type::STATUS_ACK] =
        MsgInfo("STATUS-ACK",                 "1.0");
    m[Type::STATUS_DATA] =
        MsgInfo("STATUS-DATA",                "1.0");
    m[Type::STATUS_RPT] =
        MsgInfo("STATUS-REPORT",              "1.0");
    m[Type::SUBS_DATA] =
        MsgInfo("SUBS-DATA",                  "1.0");
    m[Type::SUBS_DATA_DIV] =
        MsgInfo("SUBS-DATA-DIV",              "1.0");
    m[Type::SUBS_DATA_FLEET] =
        MsgInfo("SUBS-DATA-FLEET",            "1.0");
    m[Type::SUBS_DATA_GROUP] =
        MsgInfo("SUBS-DATA-GROUP",            "1.0");
    m[Type::SUBS_DATA_VPN] =
        MsgInfo("SUBS-DATA-VPN",              "1.0");
    m[Type::SYS_ERROR] =
        MsgInfo("SYS-ERROR",                  "1.0");
    m[Type::SYS_KEEPALIVE] =
        MsgInfo("SYS-KEEPALIVE",              "1.0");
    m[Type::VERSION_CLIENT] =
        MsgInfo("VERSION-CLIENT",             "1.0");
    m[Type::VERSION_MAP] =
        MsgInfo("VERSION-MAP",                "1.0");
    m[Type::VOIP_SERVER_REGISTERED] =
        MsgInfo("VOIP-SERVER-REGISTERED",     "1.0");
    m[Type::VOIP_SERVER_UNREGISTERED] =
        MsgInfo("VOIP-SERVER-UNREGISTERED",   "1.0");
#ifdef APP_STM
    m[Type::GRP_LINK] =
        MsgInfo("GRP-LINK",                   "1.0");
    m[Type::SUBS_DEREG] =
        MsgInfo("SUBS-DEREG",                 "1.0");
    m[Type::SUBS_REG] =
        MsgInfo("SUBS-REG",                   "1.0");
    m[Type::VOIP_OPEN_CHANNEL] =
        MsgInfo("VOIP-OPEN-CHANNEL",          "1.0");
#endif
    srand(time(NULL)); //need to seed one time at startup
    return m;
}

MsgSp::FieldNameMapT MsgSp::createFieldNameMap()
{
    FieldNameMapT m;
    m[Field::UNDEFINED]                    = "UNDEFINED";
    m[Field::ACK_REQ_FR_AFFECTED_USER]     = "Ack-Requested-Fr-Affected-Users";
    m[Field::ACK_REQUIRED]                 = "Ack-Required";
    m[Field::AFFECTED_USER]                = "Affected-User";
    m[Field::AFFECTED_USER_TYPE]           = "Affected-User-Type";
    m[Field::AI_ENCR_CTRL]                 = "AI-Encryption-Control";
    m[Field::ASG_DEASSIGN_STATUS]          = "Assign-Deassign-Status";
    m[Field::ASG_DUR]                      = "Assignment-Duration";
    m[Field::ASG_REP_RATE]                 = "Assignment-Repeat-Rate";
    m[Field::CALL_ID]                      = "Call-Id";
    m[Field::CALL_OWNERSHIP]               = "Call-Ownership";
    m[Field::CALL_PRIORITY]                = "Call-Priority";
    m[Field::CALL_QUEUED]                  = "Call-Queued";
    m[Field::CALL_STATUS]                  = "Call-Status";
    m[Field::CALL_TIMEOUT]                 = "Call-Time-Out";
    m[Field::CALL_TIMEOUT_SETUP]           = "Call-Timeout-Setup";
    m[Field::CALLED_PARTY]                 = "Called-Party";
    m[Field::CALLED_PARTY_FLEET]           = "Called-Party-Fleet";
    m[Field::CALLED_PARTY_GW]              = "Called-Party-Gateway";
    m[Field::CALLED_PARTY_TYPE]            = "Called-Party-Type";
    m[Field::CALLED_PARTY_VPN]             = "Called-Party-VPN";
    m[Field::CALLING_PARTY]                = "Calling-Party";
    m[Field::CALLING_PARTY_CELL]           = "Calling-Party-Cell";
    m[Field::CALLING_PARTY_FLEET]          = "Calling-Party-Fleet";
    m[Field::CALLING_PARTY_GW]             = "Calling-Party-Gateway";
    m[Field::CALLING_PARTY_TYPE]           = "Calling-Party-Type";
    m[Field::CALLING_PARTY_VPN]            = "Calling-Party-VPN";
    m[Field::CELL]                         = "Cell";
    m[Field::CHALLENGE]                    = "Challenge";
    m[Field::CHECKSUM]                     = "Checksum";
    m[Field::CIRCUIT_MODE_TYPE]            = "Circuit-Mode-Type";
    m[Field::CLASS_OF_USAGE]               = "Class-Of-Usage";
    m[Field::CLIR_CTRL]                    = "CLIR-Control";
    m[Field::COMM_TYPE]                    = "Communication-Type";
    m[Field::CONFIGURED_CALL_FWD_TYPE]     = "Configured-Call-Forwarding-Type";
    m[Field::DB_ADDRESS]                   = "DB-Address";
    m[Field::DB_NAME]                      = "DB-Name";
    m[Field::DB_PASSWORD]                  = "DB-Password";
    m[Field::DB_PORT]                      = "DB-Port";
    m[Field::DB_USERNAME]                  = "DB-Username";
    m[Field::DELIVERY_RPT_REQ]             = "Delivery-Report-Request";
    m[Field::DELIVERY_STATUS]              = "Delivery-Status";
    m[Field::DESC]                         = "Description";
    m[Field::DESC_LIST]                    = "Description-List";
    m[Field::DGNA_GRP]                     = "DGNA-Group";
    m[Field::DGNA_GRP_LIST]                = "DGNA-Group-List";
    m[Field::DGNA_GRP_TYPE]                = "DGNA-Group-Type";
    m[Field::DGNA_GSSI_LIST]               = "DGNA-GSSI-List";
    m[Field::DGNA_ISSI_LIST]               = "DGNA-ISSI-List";
    m[Field::DGNA_STATUS_ISSI_LIST]        = "DGNA-Status-ISSI-List";
    m[Field::DISCONNECT_CAUSE]             = "Disconnect-Cause";
    m[Field::DIVERTED_BASIC_SVC_INFO]      = "Diverted-Basic-Service-Info";
    m[Field::DIVISION]                     = "Division";
    m[Field::DIVISION_LIST]                = "Division-List";
    m[Field::ENCR_CTRL]                    = "Encryption-Control";
    m[Field::ENCR_FLAG]                    = "Encryption-Flag";
    m[Field::EXTN]                         = "Extension";
    m[Field::EXTN_LIST]                    = "Extension-List";
    m[Field::FILE_LIST]                    = "File-List";
    m[Field::FILE_PATH]                    = "File-Path";
    m[Field::FILE_SIZE]                    = "File-Size";
    m[Field::FLEET]                        = "Fleet";
    m[Field::FLEET_LIST]                   = "Fleet-List";
    m[Field::FWD_ADDR]                     = "Forward-Address";
    m[Field::FWD_ADDR_GW]                  = "Forward-Address-Gateway";
    m[Field::FWD_ADDR_TYPE]                = "Forward-Address-Type";
    m[Field::FWD_TO_USER]                  = "Forwarded-to-User";
    m[Field::FWD_TO_USER_FLEET]            = "Forwarded-to-User-Fleet";
    m[Field::FWD_TO_USER_GW]               = "Forwarded-to-User-Gateway";
    m[Field::FWD_TO_USER_TYPE]             = "Forwarded-to-User-Type";
    m[Field::FWD_TO_USER_VPN]              = "Forwarded-to-User-VPN";
    m[Field::GRP_AI_ENCR_MODE]             = "Group-AI-Encryption-Mode";
    m[Field::GRP_ATT_MODE]                 = "Group-Attachment-Mode";
    m[Field::GRP_ATTACH_DETACH_MODE]       = "Group-Attach-Detach-Mode";
    m[Field::GRP_ID]                       = "Group-Id";
    m[Field::GRP_LIST]                     = "Group-List";
    m[Field::GRP_NAME]                     = "Group-Name";
    m[Field::GRP_TYPE]                     = "Group-Type";
    m[Field::GSSI]                         = "GSSI";
    m[Field::HOOK_METHOD]                  = "Hook-Method";
    m[Field::ID]                           = "Id";
    m[Field::IDLE_TIME]                    = "Idle-Time";
    m[Field::INCIDENT_ID]                  = "Incident-Id";
    m[Field::INCL_PARTY]                   = "Included-Party";
    m[Field::INCL_PARTY_CELL]              = "Included-Party-Cell";
    m[Field::INCL_PARTY_FLEET]             = "Included-Party-Fleet";
    m[Field::INCL_PARTY_GW]                = "Included-Party-Gateway";
    m[Field::INCL_PARTY_TYPE]              = "Included-Party-Type";
    m[Field::INCL_PARTY_VPN]               = "Included-Party-VPN";
    m[Field::INCL_TYPE]                    = "Inclusion-Type";
    m[Field::INVALID_FIELD]                = "Invalid-Field";
    m[Field::INVALID_FIELD_VALUE]          = "Invalid-Field-Value";
    m[Field::INVALID_MSG_TYPE]             = "Invalid-Msg-Type";
    m[Field::INVOCATION_FAIL_CAUSE]        = "Invocation-Failure-Cause";
    m[Field::INVOKED_CALL_FWD_TYPE]        = "Invoked-Call-Forwarding-Type";
    m[Field::ISSI]                         = "ISSI";
    m[Field::ISSI_LIST]                    = "ISSI-List";
    m[Field::KEEPALIVE_PERIOD]             = "KeepAlive-Period";
    m[Field::LIFETIME_DEFAULT]             = "LifeTime-Default";
    m[Field::LINE_LIST]                    = "Line-List";
    m[Field::LOC_UPDATE_TYPE]              = "Location-Update-Type";
    m[Field::LOCATION_ACCURACY]            = "Location-Accuracy";
    m[Field::LOCATION_ALTITUDE]            = "Location-Altitude";
    m[Field::LOCATION_DIRECTION]           = "Location-Direction";
    m[Field::LOCATION_LAT]                 = "Location-Latitude";
    m[Field::LOCATION_LONG]                = "Location-Longitude";
    m[Field::LOCATION_TIME]                = "Location-Time";
    m[Field::LOCATION_VALID]               = "Location-Valid";
    m[Field::LOCATION_VELOCITY]            = "Location-Velocity";
    m[Field::LOCK_ACTION]                  = "Lock-Action";
    m[Field::MAC_ADDRESSES]                = "MAC-Addresses";
    m[Field::MAX_CALL_PRIORITY]            = "Maximum-Call-Priority";
    m[Field::MAX_MONITORING_GSSI]          = "Maximum-Monitoring-GSSI";
    m[Field::MAX_MONITORING_ISSI]          = "Maximum-Monitoring-ISSI";
    m[Field::MIN_CALL_PRIORITY]            = "Minimum-Call-Priority";
    m[Field::MON_CALL_KEY]                 = "Monitor-Call-Key";
    m[Field::MSG_ACK]                      = "Msg-Ack";
    m[Field::MSG_ID]                       = "Msg-Id";
    m[Field::MSG_NUM]                      = "Msg-Number";
    m[Field::MSG_REF]                      = "Msg-Reference";
    m[Field::NETWORK_TYPE]                 = "Network-Type";
    m[Field::NEW_CALL_ID]                  = "New-Call-Id";
    m[Field::NEW_PASSWORD]                 = "New-Password";
    m[Field::ORGANIZATION]                 = "Organization";
    m[Field::PARTY_SELECT]                 = "Party-Selection";
    m[Field::PASSWORD]                     = "Password";
    m[Field::PORT]                         = "Port";
    m[Field::PROTOCOL_ID]                  = "Protocol-Id";
    m[Field::REQ_TO_TX]                    = "Request-To-Tx";
    m[Field::RESTRICTED_USER]              = "Restricted-User";
    m[Field::RESTRICTED_USER_TYPE]         = "Restricted-User-Type";
    m[Field::RESULT]                       = "Result";
    m[Field::RETRY_COUNT]                  = "Retry-Count";
    m[Field::SERVED_USER]                  = "Served-User";
    m[Field::SERVED_USER_FLEET]            = "Served-User-Fleet";
    m[Field::SERVED_USER_GW]               = "Served-User-Gateway";
    m[Field::SERVED_USER_LIST]             = "Served-User-List";
    m[Field::SERVED_USER_TYPE]             = "Served-User-Type";
    m[Field::SERVED_USER_VPN]              = "Served-User-VPN";
    m[Field::SHORT_DATA_TYPE]              = "Short-Data-Type";
    m[Field::SHORT_RPT_TYPE]               = "Short-Report-Type";
    m[Field::SIMPLEX_DUPLEX]               = "Simplex-Duplex";
    m[Field::SSCAD_FLAGS]                  = "SSCAD-Flags";
    m[Field::SSCAD_INTERCEPT_CAUSE]        = "SSCAD-Interception-Cause";
    m[Field::SSCAD_INVOKE_CMD]             = "SSCAD-Invoke-Command";
    m[Field::SSCAD_RESTRICTED_DIR]         = "SSCAD-Restricted-Direction";
    m[Field::SSCAD_RESTRICTED_PEER]        = "SSCAD-Restricted-Peer";
    m[Field::SSCAD_RESTRICTED_PEER_TYPE]   = "SSCAD-Restricted-Peer-Type";
    m[Field::SSCAD_RESTRICTED_SVC]         = "SSCAD-Restricted-Service";
    m[Field::SSCAD_RESTRICTION_DEFINE_CMD] = "SSCAD-Restriction-Define-Cmd";
    m[Field::SSCAD_RESULT_SEQ_NUM]         = "SSCAD-Result-Sequence-Number";
    m[Field::SSCF_ACTIVE]                  = "SSCF-Active";
    m[Field::SSCF_DEACTIVATION_TYPE]       = "SSCF-Deactivation-Type";
    m[Field::SSCF_QUERY_TYPE]              = "SSCF-Query-Type";
    m[Field::SSCF_REJECT_CAUSE]            = "SSCF-Rejection-Cause";
    m[Field::SSCF_STATE]                   = "SSCF-State";
    m[Field::SSI_LIST]                     = "SSI-List";
    m[Field::STATUS_CODE]                  = "Status-Code";
    m[Field::STATUS_CODE_LIST]             = "Status-Code-List";
    m[Field::STATUS_TYPE_LIST]             = "Status-Type-List";
    m[Field::STATUS_TEXT_LIST]             = "Status-Text-List";
    m[Field::STORAGE]                      = "Storage";
    m[Field::STORE_FLAG]                   = "Store-Flag";
    m[Field::SUBS_CONTAINER_ID]            = "Subs-Container-Id";
    m[Field::SUBS_CONTAINER_TYPE]          = "Subs-Container-Type";
    m[Field::SUBS_CONTENT_ID]              = "Subs-Content-Id";
    m[Field::SUBS_CONTENT_TYPE]            = "Subs-Content-Type";
    m[Field::SUBS_PERMISSION]              = "Subs-Permission";
    m[Field::SUPPORTNUM]                   = "Support-Number";
    m[Field::SVC_SELECT_SHORT_FORM_RPT]    = "Svc-Selection-Short-Form-Report";
    m[Field::TERMINAL_TYPE]                = "Terminal-Type";
    m[Field::TIME]                         = "Time";
    m[Field::TIME_IN_Q]                    = "Time-In-Queue";
    m[Field::TIME_IN_TRAFFIC]              = "Time-In-Traffic";
    m[Field::TIMESTAMP]                    = "Timestamp";
    m[Field::TOTAL_MESSAGES]               = "Total-Messages";
    m[Field::TX_DEMAND_PRIORITY]           = "Tx-Demand-Priority";
    m[Field::TX_GRANT]                     = "Tx-Grant";
    m[Field::TX_PARTY]                     = "Tx-Party";
    m[Field::TX_PARTY_CELL]                = "Tx-Party-Cell";
    m[Field::TX_PARTY_FLEET]               = "Tx-Party-Fleet";
    m[Field::TX_PARTY_GW]                  = "Tx-Party-Gateway";
    m[Field::TX_PARTY_TYPE]                = "Tx-Party-Type";
    m[Field::TX_PARTY_VPN]                 = "Tx-Party-VPN";
    m[Field::TX_REQ_PERMISSION]            = "Tx-Request-Permission";
    m[Field::TYPE]                         = "Type";
    m[Field::USER_DATA]                    = "User-Data";
    m[Field::USER_DATA_LENGTH]             = "User-Data-Length";
    m[Field::USER_DEFINED_DATA_1]          = "User-Defined-Data-1";
    m[Field::USER_DEFINED_DATA_2]          = "User-Defined-Data-2";
    m[Field::USER_DEFINED_DATA_3]          = "User-Defined-Data-3";
    m[Field::USER_TYPE]                    = "User-Type";
    m[Field::USERNAME]                     = "Username";
    m[Field::VALIDITY_PERIOD]              = "Validity-Period";
    m[Field::VERSION]                      = "Version";
    m[Field::VOIP_AUDIO_RTP_KEY]           = "VoIP-Audio-RTP-Key";
    m[Field::VOIP_AUDIO_RTP_LKEY]          = "VoIP-Audio-RTP-LocalKey";
    m[Field::VOIP_AUDIO_RTP_LPORT]         = "VoIP-Audio-RTP-LocalPort";
    m[Field::VOIP_AUDIO_RTP_PORT]          = "VoIP-Audio-RTP-Port";
    m[Field::VOIP_GW]                      = "VoIP-Gateway";
    m[Field::VOIP_SSRC]                    = "VoIP-SSRC";
    m[Field::VOIP_VIDEO_RTP_KEY]           = "VoIP-Video-RTP-Key";
    m[Field::VOIP_VIDEO_RTP_LKEY]          = "VoIP-Video-RTP-LocalKey";
    m[Field::VOIP_VIDEO_RTP_LPORT]         = "VoIP-Video-RTP-LocalPort";
    m[Field::VOIP_VIDEO_RTP_PORT]          = "VoIP-Video-RTP-Port";
    m[Field::VPN]                          = "VPN";
#ifdef APP_STM
    m[Field::AREA_SELECT]                  = "Area-Select";
    m[Field::CTRL_ENTITY]                  = "Controlling-Entity";
    m[Field::INTR_PARTY]                   = "Intr-Party";
    m[Field::INTR_PARTY_TYPE]              = "Intr-Party-Type";
    m[Field::KPI_MCI]                      = "KPI-MCI";
    m[Field::MC_SUBS_REQ]                  = "MC-Subscribers-Requested";
    m[Field::NOTIF_IND]                    = "Notif-Ind";
    m[Field::QOS]                          = "QOS";
    m[Field::SLOTS_PER_FRAME]              = "Slots-Per-Frame";
    m[Field::SPEECH_SVC]                   = "Speech-Svc";
    m[Field::USER_DEFINED_DATA_4]          = "User-Defined-Data-4";
#endif
    return m;
}
