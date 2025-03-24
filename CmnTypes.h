/**
 * Collection of common types and related conversion functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: CmnTypes.h 1908 2025-03-05 00:54:00Z rosnin $
 * @author Nurulashikhin
 */
#ifndef CMNTYPES_H
#define CMNTYPES_H

namespace CmnTypes
{
    enum eActionType
    {
        //a new entry must also be added to QtUtils::getActionIconSrc(), and
        //QtUtils::addMenuAction() if necessary
        ACTIONTYPE_ADD,
        ACTIONTYPE_ATT,
        ACTIONTYPE_CALL,
        ACTIONTYPE_CLEAR,
        ACTIONTYPE_CLEAR_UNC_GRP_ATT,
        ACTIONTYPE_COMMSREG,
        ACTIONTYPE_CONTACT_ADD,
        ACTIONTYPE_CONTACT_SHOW,
        ACTIONTYPE_COPY,
        ACTIONTYPE_COPY_PATH,
        ACTIONTYPE_DELETE,
        ACTIONTYPE_DELETE_SEL,
        ACTIONTYPE_DGNA_ASSG,
        ACTIONTYPE_DGNA_DISSOLVE,
        ACTIONTYPE_EDIT,
        ACTIONTYPE_GIS,
        ACTIONTYPE_GRP_ATT,
        ACTIONTYPE_INCIDENT,
        ACTIONTYPE_LOCATE,
        ACTIONTYPE_LOGOUT,
        ACTIONTYPE_MMS,
        ACTIONTYPE_MMS_DOWNLOAD,
        ACTIONTYPE_MON,
        ACTIONTYPE_MONSTART,
        ACTIONTYPE_MONSTOP,
        ACTIONTYPE_MSG,
        ACTIONTYPE_NOTES,
        ACTIONTYPE_OSK,
        ACTIONTYPE_PB_ADD,
        ACTIONTYPE_PB_DEL,
        ACTIONTYPE_PLAY,
        ACTIONTYPE_PLAY_VID,
        ACTIONTYPE_PRINT_EXCEL,
        ACTIONTYPE_PRINT_PDF,
        ACTIONTYPE_PRINT_PRV,
        ACTIONTYPE_PTT,
        ACTIONTYPE_PTT_ACTIVE,
        ACTIONTYPE_PTT_PENDING,
        ACTIONTYPE_REFRESH,
        ACTIONTYPE_RENAME,
        ACTIONTYPE_REPORT,
        ACTIONTYPE_RESOURCES,
        ACTIONTYPE_RSC_SELECT,
        ACTIONTYPE_SAVE,
        ACTIONTYPE_SAVE_ALL,
        ACTIONTYPE_SDS,
        ACTIONTYPE_SELECT_ALL,
        ACTIONTYPE_SELECT_NONE,
        ACTIONTYPE_SETTINGS,
        ACTIONTYPE_SORT,
        ACTIONTYPE_STATUS,
        ACTIONTYPE_MAX = ACTIONTYPE_STATUS
    };

    enum eCallType
    {
        CALLTYPE_BROADCAST_IN,
        CALLTYPE_BROADCAST_OUT,
        CALLTYPE_DISPATCHER,
        CALLTYPE_GROUP_IN,
        CALLTYPE_GROUP_OUT,
        CALLTYPE_IND_AMBIENCE,
        CALLTYPE_IND_IN,
        CALLTYPE_IND_OUT,
        CALLTYPE_MOBILE,
        CALLTYPE_MON_AMBIENCE,
        CALLTYPE_MON_IND_DUPLEX,
        CALLTYPE_MON_IND_PTT
    };

    //call/message directions and message types
    enum eCommsType
    {
        //a new entry must also be added to QtUtils::getCommsIconSrc()
        COMMS_DIR_IN,       //incoming
        COMMS_DIR_MISSED,   //missed call
        COMMS_DIR_MON,      //monitored
        COMMS_DIR_OUT,      //outgoing
        COMMS_MSG_SDS,
        COMMS_MSG_STATUS,
        COMMS_MSG_MMS,
        COMMS_MAX = COMMS_MSG_MMS
    };

    enum eIdType
    {
        //do not change positions because phonebook stores the values in db,
        //and a new entry must also be added to QtUtils::getRscIconSrc()
        IDTYPE_DGNA_GRP,
        IDTYPE_DGNA_IND,
        IDTYPE_DISPATCHER,
        IDTYPE_GROUP,
        IDTYPE_SUBSCRIBER,
        IDTYPE_MOBILE,
        IDTYPE_MOBILE_ONLINE,
        IDTYPE_UNKNOWN //MUST be at the last position
    };

    /**
     * Converts a MsgSp Identity Type value to eIdType.
     *
     * @param[in] type The Identity Type.
     * @return The ID type.
     */
    int fromMsgSpIdentityType(int type);

    /**
     * Converts an eIdType value to MsgSp Identity Type.
     *
     * @param[in] type The ID type.
     * @return The MsgSp Identity Type.
     */
    int toMsgSpIdentityType(int type);

    /**
     * Converts a MsgSp Subscriber Type value to eIdType.
     *
     * @param[in] type The Subscriber Type.
     * @return The ID type.
     */
    int fromMsgSpSubsType(int type);

    /**
     * Converts an eIdType value to MsgSp Subscriber Type.
     *
     * @param[in] type The ID type.
     * @return The MsgSp Subscriber Type.
     */
    int toMsgSpSubsType(int type);

    /**
     * Checks whether a MsgSp Identity Type value is for a dispatcher.
     *
     * @param[in] identityType The identity type.
     * @return true if the identity type is dispatcher.
     */
    bool isIdentityTypeDispatcher(int identityType);
}
#endif //CMNTYPES_H
