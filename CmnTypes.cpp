/**
 * Type conversion functions implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: CmnTypes.cpp 1871 2024-08-13 08:02:27Z rosnin $
 * @author Nurulashikhin
 */
#include "MsgSp.h"
#include "CmnTypes.h"

int CmnTypes::fromMsgSpIdentityType(int type)
{
    switch (type)
    {
        case MsgSp::Value::IDENTITY_TYPE_DISPATCHER:
            return IDTYPE_DISPATCHER;
        case MsgSp::Value::IDENTITY_TYPE_GSSI:
            return IDTYPE_GROUP;
        case MsgSp::Value::IDENTITY_TYPE_ISSI:
            return IDTYPE_SUBSCRIBER;
        case MsgSp::Value::IDENTITY_TYPE_MOBILE:
            return IDTYPE_MOBILE;
        default:
            return IDTYPE_UNKNOWN;
    }
}

int CmnTypes::toMsgSpIdentityType(int type)
{
    switch (type)
    {
        case IDTYPE_DGNA_GRP:
        case IDTYPE_DGNA_IND:
        case IDTYPE_GROUP:
            return MsgSp::Value::IDENTITY_TYPE_GSSI;
        case IDTYPE_DISPATCHER:
            return MsgSp::Value::IDENTITY_TYPE_DISPATCHER;
        case IDTYPE_MOBILE:
        case IDTYPE_MOBILE_ONLINE:
            return MsgSp::Value::IDENTITY_TYPE_MOBILE;
        case IDTYPE_SUBSCRIBER:
            return MsgSp::Value::IDENTITY_TYPE_ISSI;
        default:
            return MsgSp::Value::IDENTITY_TYPE_UNKNOWN;
    }
}

int CmnTypes::fromMsgSpSubsType(int type)
{
    switch (type)
    {
        case MsgSp::Value::SUBS_TYPE_DGNA_GRP:
            return IDTYPE_DGNA_GRP;
        case MsgSp::Value::SUBS_TYPE_DGNA_IND:
            return IDTYPE_DGNA_IND;
        case MsgSp::Value::SUBS_TYPE_DISPATCHER:
            return IDTYPE_DISPATCHER;
        case MsgSp::Value::SUBS_TYPE_GRP:
            return IDTYPE_GROUP;
        case MsgSp::Value::SUBS_TYPE_ISSI:
            return IDTYPE_SUBSCRIBER;
        case MsgSp::Value::SUBS_TYPE_MOBILE:
            return IDTYPE_MOBILE;
        default:
            return IDTYPE_UNKNOWN;
    }
}

int CmnTypes::toMsgSpSubsType(int type)
{
    switch (type)
    {
        case IDTYPE_DGNA_GRP:
            return MsgSp::Value::SUBS_TYPE_DGNA_GRP;
        case IDTYPE_DGNA_IND:
            return MsgSp::Value::SUBS_TYPE_DGNA_IND;
        case IDTYPE_DISPATCHER:
            return MsgSp::Value::SUBS_TYPE_DISPATCHER;
        case IDTYPE_GROUP:
            return MsgSp::Value::SUBS_TYPE_GRP;
        case IDTYPE_MOBILE:
        case IDTYPE_MOBILE_ONLINE:
            return MsgSp::Value::SUBS_TYPE_MOBILE;
        case IDTYPE_SUBSCRIBER:
            return MsgSp::Value::SUBS_TYPE_ISSI;
        default:
            return MsgSp::Value::SUBS_TYPE_UNKNOWN;
    }
}

bool CmnTypes::isIdentityTypeDispatcher(int identityType)
{
    return (identityType == MsgSp::Value::IDENTITY_TYPE_DISPATCHER);
}
