/**
 * Subscriber data manager.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: SubsData.h 1871 2024-08-13 08:02:27Z rosnin $
 * @author Zahari Hadzir
 */
#ifndef SUBSDATA_H
#define SUBSDATA_H

#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <time.h>   //time_t, time()

#include "MsgSp.h"
#include "PalLock.h"

enum eSubsType
{
    SUBS_VPN_DIV_LIST,
    SUBS_DIV_FLEET_LIST,
    SUBS_FLEET_ISSI_LIST,
    SUBS_FLEET_MOB_LIST,
    //SUBS_GSSI_FLEET_LIST is meaningful only on server - each static group
    //contains the fleets as per NEBULA FGT
    SUBS_GSSI_FLEET_LIST,
    SUBS_GSSI_SSI_LIST,
    SUBS_GSSI_ATTACH_LIST,
    //client only: unconfirmed grp attachments based on grp SDS/Status and call
    //setup/PTT - mainly to cater for ISSIs in nwk with no grp attachment
    //visibility
    SUBS_GSSI_ATTACH_LIST_UNC
};

class SubsData
{
public:
    enum
    {
        FLEET_NONE = 0
    };

    enum eState
    {
        STATE_INVALID,
        STATE_INIT,
        STATE_DOWNLOADING,
        STATE_READY
    };

    enum eTerminalType
    {
        //values must match DB
        TERMINALTYPE_HANDHELD,
        TERMINALTYPE_DEFAULT = TERMINALTYPE_HANDHELD,
        TERMINALTYPE_VEHICULAR,
        TERMINALTYPE_BRIEFCASE,
        TERMINALTYPE_COVERT,
        TERMINALTYPE_DESKTOP,
        TERMINALTYPE_MARINEKIT,
        TERMINALTYPE_RADIODISP,
        TERMINALTYPE_RIM,
        TERMINALTYPE_RUCKSACK,
        TERMINALTYPE_PHONE,
        TERMINALTYPE_BWC
    };

    typedef std::set<int>              IdSetT;
    //dispatcher IDs and their VOIP domain (IP or name)
    typedef std::map<int, std::string> Id2DomMapT;
    //subscriber ID list for each key (VPN, DIV, FLEET, etc.)
    typedef std::map<int, IdSetT>      Int2IdsMapT;
    //subscriber data for each key (eSubsType)
    typedef std::map<int, Int2IdsMapT> IdMapT;

#ifdef SERVERAPP
    enum eBranchIdType
    {
        BRID_MIN, //for validation
        BRID_DISP = BRID_MIN,
        BRID_EM,
        BRID_MOB,
        BRID_SSI,
        BRID_SVR,
        BRID_INVALID
    };

    struct SsiData
    {
        SsiData() {}
        SsiData(bool f, const std::string &n) :
            locName((f)? n: ""), nwkName((f)? "": n) {}

        const std::string &name() const
        {
            if (!locName.empty())
                return locName;
            return nwkName;
        }

        //subscriber or group name in hexadecimal-encoded UTF8 Unicode,
        //e.g. "Sapura 1" is "5361707572612031"
        std::string locName; //local name in dispatcher
        std::string nwkName; //network name
    };
    //description of each ISSI and GSSI
    typedef std::map<int, SsiData> Ssi2DescMapT;

#else
    //group membership status
    enum eGrpStat
    {
        GRP_STAT_NONE,
        GRP_STAT_ASSIGN,    //group has assigned members
        GRP_STAT_ATTACH     //group has attached members
    };

    typedef std::map<int, std::string> Ssi2DescMapT;
    //branch ID names in hexadecimal-encoded UTF8 Unicode
    typedef std::map<int, std::string> BranchMapT;
#endif //SERVERAPP
    //key is eTerminalType
    typedef std::map<int, Ssi2DescMapT> IssiType2DescMapT;

    /**
     * Performs memory cleanup.
     */
    static void final();

    /**
     * Gets the current data update timestamp.
     *
     * @return The timestamp.
     */
    static time_t getTimestamp();

    /**
     * Gets the current data update timestamp as formatted string:
     *    "Www Mmm dd hh:mm:ss yyyy\n"
     * where Www is the weekday, Mmm the month (in letters).
     * E.g. "Tue Dec 31 15:11:16 2013".
     * With raw time: "1388473876 Tue Dec 31 15:11:16 2013".
     *
     * @param[in] showRaw true to show the raw time.
     * @return The timestamp.
     */
    static std::string getTimestampStr(bool showRaw = false);

    /**
     * Processes SUBS_DATA_* messages.
     * Processes MON-SUBS-* or MON-SSDGNA-* message if the Subscriber data
     * downloading is completed. Otherwise adds it to queue for later
     * processing.
     *
     * @param[in] msg The message.
     * @return false if msg queued or discarded.
     */
    static bool processMsg(MsgSp *msg);

#ifdef SERVERAPP
    /**
     * Sets Fleet ID and clears all data.
     *
     * @param[in] fleet The fleet ID.
     */
    static void init(int fleet);

    /**
     * Sets Fleet ID. Mainly for STM-network mode after receiving the ID.
     *
     * @param[in] fleet The fleet ID.
     */
    static void setFleet(int fleet)
    {
        if (fleet >= 0)
            sFleet = fleet;
    }

    /**
     * Adds Subscriber data elements of a given type.
     *
     * @param[in] type The type. See eSubsType.
     * @param[in] key  The container ID (VPN/Division/Fleet/GSSI).
     * @param[in] data The elements.
     */
    static void addList(int type, int key, const std::vector<int> &data);

    /**
     * Adds ISSIs to a fleet, and assigns the VPN and fleet to each ISSI.
     *
     * @param[in] vpn   The VPN ID.
     * @param[in] fleet The Fleet ID.
     * @param[in] data  The ISSIs.
     * @param[in] types The ISSI types from MsgSp::Value::ISSI_TYPE_*.
     * @param[in] desc  The descriptions.
     */
    static void addFleetIssis(int                             vpn,
                              int                             fleet,
                              const std::vector<int>         &data,
                              const std::vector<int>         &types,
                              const std::vector<std::string> &desc);

    /**
     * Adds or removes a mobile ISSI to/from a fleet.
     *
     * @param[in] add   true to add.
     * @param[in] issi  The ISSI.
     * @param[in] fleet The Fleet ID. Omit to use the assigned fleet.
     * @return true if successful.
     */
    static bool setFleetMob(bool add, int issi, int fleet = -1);

    /**
     * Checks whether data contain mobiles.
     *
     * @return true if mobiles present.
     */
    static bool hasMobiles();

    /**
     * Adds groups. Only for STM-network, with data from DB.
     * Input vectors should have the same size.
     *
     * @param[in] grpType The fleets.
     * @param[in] gssis   The GSSIs.
     * @param[in] names   The names.
     */
    static void addGrps(const std::vector<int>         &fleets,
                        const std::vector<int>         &gssis,
                        const std::vector<std::string> &names);

    /**
     * Adds groups to a VPN.
     *
     * @param[in] vpn     The VPN ID.
     * @param[in] grpType The group type. See MsgSp::Value::GRP_TYPE_*.
     * @param[in] gssis   The group GSSIs.
     * @param[in] names   The group names.
     */
    static void addVpnGrps(int                             vpn,
                           int                             grpType,
                           const std::vector<int>         &gssis,
                           const std::vector<std::string> &names);

    /**
     * Adds fleets to a static group, and assigns the group to each fleet.
     *
     * @param[in] gssi   The GSSI.
     * @param[in] fleets The fleets.
     */
    static void addGrpFleets(int gssi, const std::vector<int> &fleets);

    /**
     * Adds member ISSIs to a group.
     *
     * @param[in] gssi    The group.
     * @param[in] grpType The group type. See MsgSp::Value::GRP_TYPE_*.
     * @param[in] issis   The members.
     */
    static void addGrpIssis(int                     gssi,
                            int                     grpType,
                            const std::vector<int> &issis);

    /**
     * Adds member GSSIs to a DGNA-Grp.
     *
     * @param[in] gssi  The group.
     * @param[in] gssis The members.
     */
    static void addGrpGssis(int gssi, const std::vector<int> &gssis);

    /**
     * Sets the local name of a group.
     *
     * @param[in] updateTimestamp true to update the data timestamp if the
     *                            name is actually updated.
     * @param[in] gssi            The GSSI.
     * @param[in] name            The group name.
     * @param[in] isHex           true if the name is already in hexadecimal
     *                            format. Otherwise it is to be converted.
     * @return true if the local name is updated for a valid network GSSI.
     */
    static bool setGssiDescLocal(bool               updateTimestamp,
                                 int                gssi,
                                 const std::string &name,
                                 bool               isHex = false);

    /**
     * Sets the local name of a subscriber.
     *
     * @param[in] updateTimestamp true to update the data timestamp if the
     *                            name is actually updated.
     * @param[in] type            The terminal type. See eTerminalType.
     * @param[in] issi            The ISSI.
     * @param[in] name            The subscriber name.
     * @param[in] isHex           true if the name is already in hexadecimal
     *                            format. Otherwise it is to be converted.
     * @return true if the local name is updated for a valid network ISSI.
     */
    static bool setIssiDescLocal(bool               updateTimestamp,
                                 int                type,
                                 int                issi,
                                 const std::string &name,
                                 bool               isHex = false);

    /**
     * Gets the network name of a group.
     *
     * @param[in] gssi The GSSI.
     * @return The name, or empty string if not found.
     */
    static std::string getGssiDescNwk(int gssi);

    /**
     * Gets the network name of a subscriber.
     *
     * @param[in] type The terminal type - eTerminalType.
     * @param[in] issi The ISSI.
     * @return The name, or empty string if not found.
     */
    static std::string getIssiDescNwk(int type, int issi);

    /**
     * In fleet mode, sets the VPN ID based on the given division.
     *
     * @param[in] div The division.
     * @return The VPN ID.
     */
    static int setVpn(int div);

    /**
     * Gets all VPNs.
     *
     * @param[out] data The container to insert the VPN IDs. Existing content
     *                  is not overwritten.
     */
    static void getVpns(IdSetT &data);

    /**
     * Gets VPN and fleet of an ISSI.
     *
     * @param[in]  issi  The ISSI.
     * @param[out] vpn   The VPN ID.
     * @param[out] fleet The Fleet ID.
     * @return true if successful.
     */
    static bool getIssiVpnFleet(int issi, int &vpn, int &fleet);

    /**
     * Gets Subscriber data elements of a given type.
     *
     * @param[in]  type The type. See eSubsType.
     * @param[in]  key  The container ID (VPN/Division/Fleet/GSSI).
     * @param[out] data The elements. Existing content is overwritten.
     * @return true if successful.
     */
    static bool getDataSet(int type, int key, IdSetT &data);

    /**
     * Gets the complete Subscriber data. Overwrites existing content of all
     * output containers.
     *
     * @param[out] subsData    Main subscriber data.
     * @param[out] vpnGroups   DGNA groups and empty static groups.
     * @param[out] fleetGroups Static groups in each fleet.
     * @param[out] ssiDesc     Description of each SSI.
     * @param[in]  cid         The requesting dispatcher client ID, to determine
     *                         the relevant branches.
     * @return true if successful.
     */
    static bool getData(IdMapT       &subsData,
                        IdMapT       &vpnGroups,
                        Int2IdsMapT  &fleetGroups,
                        Ssi2DescMapT &ssiDesc,
                        int           cid);

    /**
     * Gets the group descriptions.
     *
     * @param[out] desc    The descriptions.
     * @param[in]  cid     The requesting client ID.
     * @param[in]  dispReq true if requesting client is dispatcher.
     * @return true if successful.
     */
    static bool getGrpDesc(Ssi2DescMapT &desc, int cid, bool dispReq);

    /**
     * Gets all client IDs.
     *
     * @param[in]  disp    true for dispatcher clients. Otherwise mobile clients.
     * @param[in]  cid     The requesting client ID.
     * @param[in]  dispReq true if requesting client is dispatcher.
     * @param[out] ids     Dispatcher/mobile IDs. Existing content overwritten.
     * @return true if output not empty.
     */
    static bool getClientData(bool        disp,
                              int         cid,
                              bool        dispReq,
                              Id2DomMapT &ids);

    /**
     * Gets the terminal types in current subscriber data.
     *
     * @param[out] typeList The terminal types - eTerminalType.
     */
    static void getTerminalTypes(IdSetT &typeList);

    /**
     * Filters an ISSI list by terminal type.
     *
     * @param[in]  type    The terminal type. See eTerminalType.
     * @param[in]  inList  ISSIs of various terminal types.
     * @param[out] outList The filtered ISSIs of the specified type. Cleared
     *                     on entry.
     * @return true if output not empty.
     */
    static bool filterIssis(int type, const IdSetT &inList, IdSetT &outList);

    /**
     * Checks a Subscriber data list.
     *
     * @param[in] type The type. See eSubsType.
     * @param[in] key  The container ID (VPN/Division/Fleet/GSSI).
     * @return true if empty.
     */
    static bool isListEmpty(int type, int key);

    /**
     * Gets the fleet of an ISSI when the caller is not holding sDataLock.
     *
     * @param[in] issi The ISSI.
     * @return The fleet ID. Negative if not found, indicating invalid ISSI.
     */
    static int getIssiFleet(int issi);

    /**
     * Checks whether a group is in a particular fleet.
     *
     * @param[in] fleet The fleet.
     * @param[in] gssi  The GSSI.
     * @return true if the group is a member of the fleet.
     */
    static bool isGssiInFleet(int fleet, int gssi);

    /**
     * Adds a fleet branch, which is defined by its unique ID, unique name, and
     * cluster.
     * Branches in the same cluster are peer branches.
     *  -E.g. for different dispatching sites of an agency.
     * Branches in different clusters are independent branches.
     *  -For independent sub-units of a fleet, e.g. subdivisions of the army.
     * Each resource (ISSI/GSSI) is assigned to one or more branches.
     * Each client is assigned to zero or more branches. All branches in the
     * same cluster as the assigned branches comprise its allowed branches.
     * Client may attach to zero or more allowed branches, and sees resources
     * within the attached branches only. If attached to none in single-cluster
     * system, it sees all.
     * Possible fleet branch setup:
     *  1. Single-cluster
     *     -Cluster need not be specified here, all use default.
     *     -ISSI/GSSI not assigned to any branch is seen by all clients.
     *     -Client not attached to any branch sees all resources.
     *  2. Multi-cluster with only one branch per cluster
     *     -Unique cluster for each.
     *     -ISSI/GSSI not assigned to any branch is not seen by any client.
     *     -Client not attached to any branch sees no resource.
     *  3. Combination, i.e. multi-cluster with multiple branches
     *     -ISSI/GSSI not assigned to any branch is not seen by any client.
     *     -Client not attached to any branch sees no resource.
     *
     * @param[in] branch  The branch ID - must be unique.
     * @param[in] cluster The cluster. Negative to use default.
     * @param[in] name    The name - must be unique.
     * @return true if successful, false if the branch or name already exists.
     */
    static bool addBranch(int branch, int cluster, const std::string &name);

    /**
     * Modifies data of a branch.
     *
     * @param[in]  branch  The branch.
     * @param[in]  cluster The cluster. Negative if unchanged.
     * @param[in]  name    The branch name. Empty if unchanged.
     * @param[out] affCids Affected client IDs.
     * @return true if data modified.
     */
    static bool editBranch(int                branch,
                           int                cluster,
                           const std::string &name,
                           IdSetT            &affCids);

    /**
     * Removes a branch.
     *
     * @param[in]  branch  The branch.
     * @param[out] affCids Affected client IDs.
     */
    static void removeBranch(int branch, IdSetT &affCids);

    /**
     * Gets all branches.
     *
     * @param[out] branches The branches. Cleared on entry.
     * @return true if output not empty.
     */
    static bool getBranches(IdSetT &branches);

    /**
     * Gets a list of all branches.
     * Output format:
     *  <branch ID> <name> <cluster>
     *  <branch ID> <name> <cluster>
     * Cluster shown only in multi-cluster system.
     *
     * @return The list.
     */
    static std::string getBranches();

    /**
     * Gets information for a branch - cluster and name.
     *
     * @param[in]  branch The branch.
     * @param[out] name   The branch name converted from hex format.
     * @return The cluster, or -1 if branch not found.
     */
    static int getBranchInfo(int branch, std::string &name);

    /**
     * Assigns client/resource IDs of a particular type to a branch.
     *
     * @param[in]  branch  The branch.
     * @param[in]  type    ID type - eBranchIdType.
     * @param[in]  idList  Space-separated ID ranges as expected by
     *                     Utils::fromStringWithRange().
     * @param[out] affCids Affected client IDs.
     * @return true if successful.
     */
    static bool addBranchIds(int                branch,
                             int                type,
                             const std::string &idList,
                             IdSetT            &affCids);

    /**
     * Removes client/resource IDs from a branch.
     *
     * @param[in] branch   The branch.
     * @param[in] type     ID type - eBranchIdType.
     * @param[in] idList   Space-separated ID ranges as expected by
     *                     Utils::fromStringWithRange(). Omit to remove all.
     * @param[out] affCids Affected client IDs.
     */
    static void removeBranchIds(int                branch,
                                int                type,
                                const std::string &idList,
                                IdSetT            &affCids);

    /**
     * Interface to getBranchIdsHaveLock().
     * Caller must not be holding sDataLock.
     *
     * @param[in] branch The branch.
     * @param[in] type   The ID type - eBranchIdType.
     * @return See getBranchIdsHaveLock().
     */
    static std::string getBranchIds(int branch, int type);

    /**
     * Gets client IDs of a particular type in the given branches, only in
     * multi-cluster system. For single cluster, this returns nothing.
     * Caller must not be holding sDataLock.
     *
     * @param[in] branches The branches.
     * @param[in] type     The ID type - eBranchIdType.
     * @return See getBranchIdsHaveLock().
     */
    static std::string getBranchIds(const IdSetT &branches, int type);

    /**
     * Interface to isInBranchHaveLock(const IdSetT ...).
     * Caller must not be holding sDataLock.
     *
     * @param[in] branches The branches.
     * @param[in] type     The ID type - eBranchIdType.
     * @param[in] id       The ID.
     * @return See isInBranchHaveLock().
     */
    static bool isInBranch(const IdSetT &branches, int type, int id);

    /**
     * Interface to isInBranchHaveLock(int ...).
     * Caller must not be holding sDataLock.
     *
     * @param[in] type   ID type - eBranchIdType.
     * @param[in] id     The ID.
     * @param[in] branch The branch. Omit to find in all branches.
     * @return See isInBranchHaveLock().
     */
    static bool isInBranch(int type, int id, int branch = 0);

    /**
     * Gets branches containing a client/resource ID.
     *
     * @param[in]  id       The ID.
     * @param[out] branches The branches.
     * @param[in]  type     The ID type - eBranchIdType. Omit if unknown.
     * @return false if either no branch defined or ID is linked to all branches
     *         (output empty in both cases).
     *         true otherwise - but with empty output for unassigned ID in
     *         multi-cluster system, which means it is not usable.
     */
    static bool getIdBranches(int id, IdSetT &branches, int type = -1);

    /**
     * Gets branches relevant to a server user ID.
     * If request is for all, or in single-cluster system, gets all branches.
     * Otherwise determines the cluster that contains the ID, and gets all its
     * branches.
     *
     * @param[in]  usrId    The user ID.
     * @param[out] branches The branches.
     * @param[in]  all      true to get all branches.
     * @return false if either no branch or single branch defined.
     *         true otherwise - but with empty output for unassigned ID in
     *         multi-cluster system, which means it is not usable.
     */
    static bool getIdBranches(const std::string &usrId,
                              IdSetT            &branches,
                              bool               all = false);

    /**
     * Gets a list of branches containing each given client/resource ID.
     * Output format:
     *  Type 2          (line shown only if type >= 0)
     *  3101000: 1
     *  3102000: 2 3
     *
     * @param[in] idList Space-separated ID ranges. Maximum 50 IDs processed.
     * @param[in] type   The ID type - eBranchIdType. Omit if unknown.
     * @return Space-separated branches for each ID.
     */
    static std::string getIdBranches(const std::string &idList, int type = -1);

    /**
     * Checks whether a server user has permission for an ISSI or GSSI.
     * If no fleet branching or only 1 branch, this is always true.
     * Otherwise the user and SSI must be in common branches.
     *
     * @param[in] ssi The ISSI or GSSI.
     * @param[in] usr The server username.
     * @return true if valid.
     */
    static bool validSsi(int ssi, const std::string &usr);

    /**
     * Gets resource and client IDs not assigned to any branch.
     * Output format:
     *  ISSI(6): 5025987-5025990 5026101 5206102
     *  GSSI(12): 3204781 3204782 3204900-3204909
     *  Disp(2): 11004 11007
     *  Mobile(2): 1900013 1900075
     *
     * @return The unassigned IDs.
     */
    static std::string getBranchUnassignedIds();

    /**
     * Gets allowed branches for a dispatcher client.
     * This means all branches to which the client can attach, not limited to
     * the current assignments only.
     * If multiple branches found, adds them to message fields GRP_LIST and
     * DESC_LIST, to be sent to the client.
     *
     * @param[in]  cid The client ID.
     * @param[out] msg The BRANCH_DATA message.
     * @return true if data added to msg.
     */
    static bool getAllowedBranches(int cid, MsgSp &msg);

    /**
     * Adds a dispatcher client to branches in the GRP_LIST field of BRANCH_DATA
     * message (after validating the branches against allowed ones), and removes
     * it from other branches.
     * This is for a client to attach to branches among the allowed ones given
     * by getAllowedBranches().
     * If the message does not have the field in single-cluster system, removes
     * the client from all branches, making it active in all. In multi-cluster
     * system, adds the client to all allowed branches.
     *
     * @param[in] cid The client ID.
     * @param[in] msg The BRANCH_DATA message.
     * @return MsgSp::Value::RESULT_*.
     */
    static int setIdBranches(int cid, const MsgSp &msg);

    /**
     * Sets or clears the emergency SSI for mobile clients.
     * Caller must ensure that the SSI is really in the branch, if given.
     *
     * @param[in] ssi    The SSI. 0 to clear.
     * @param[in] branch The branch. 0 if no branch configured.
     */
    static void setEmSsi(int ssi, int branch = 0);

    /**
     * Gets the emergency SSI.
     *
     * @param[in] branch The branch. 0 if no branch configured.
     */
    static int getEmSsi(int branch = 0);

    /**
     * Gets the emergency SSI for a mobile client.
     *
     * @param[in] id The client ID.
     * @return The SSI, or 0 if not set.
     */
    static int getClientEmSsi(int cid);

#else //SERVERAPP
    /**
     * Sets the fleet ID and clears client data.
     *
     * @param[in] fleet The fleet ID.
     */
    static void clientInit(int fleet);

    /**
     * Clears all subscriber data upon SUBS_DATA.
     * Saves branch data upon BRANCH_DATA.
     *
     * @param[in] msg SUBS_DATA or BRANCH_DATA message.
     */
    static void clientInit(MsgSp *msg);

    /**
     * Gets groups of a given type.
     *
     * @param[in]  type   The group type. See MsgSp::Value::SUBS_TYPE_*.
     * @param[out] groups The groups.
     * @return true if successful.
     */
    static bool getGroups(int type, Ssi2DescMapT &groups);

    /**
     * Gets groups by type.
     *
     * @param[out] stat    Static groups.
     * @param[out] dgnaInd DGNA groups of ISSIs.
     * @param[out] dgnaGrp DGNA groups of GSSIs.
     * @return true if successful.
     */
    static bool getGroups(Ssi2DescMapT &stat,
                          Ssi2DescMapT &dgnaInd,
                          Ssi2DescMapT &dgnaGrp);

    /**
     * Gets ISSIs that can be added to a DGNA-Ind.
     * In fleet-mode, those are the fleet ISSIs. In network-mode, those are
     * ISSIs of the DGNA-Ind's VPN.
     *
     * @param[in]  gssi  The DGNA-Ind GSSI.
     * @param[out] issis The potential member ISSIs.
     * @return true if successful.
     */
    static bool getDgnaAllowedIssis(int gssi, IdSetT &issis);

    /**
     * Gets groups that can be added to a DGNA-Grp.
     * In fleet-mode, those are the fleet groups. In network-mode, those are
     * groups of the DGNA-Grp's VPN.
     *
     * @param[in]  gssi  The DGNA-Grp GSSI.
     * @param[out] gssis The potential member groups.
     * @return true if successful.
     */
    static bool getDgnaAllowedGssis(int gssi, IdSetT &gssis);

    /**
     * Gets GSSIs from a source list, excluding those in the exclude list,
     * if any.
     *
     * @param[in]  src      The source.
     * @param[out] gssis    The destination.
     * @param[in]  excludes GSSIs to exclude.
     */
    static void getGssis(const IdSetT &src,
                         IdSetT       &gssis,
                         const IdSetT *excludes = 0);

    /**
     * Gets the terminal type of an ISSI.
     *
     * @param[in] issi The ISSI.
     * @return The type (eTerminalType), or -1 if not found.
     */
    static int getIssiType(int issi);

    /**
     * Gets a group's active membership status.
     *
     * @param[in] gssi The GSSI.
     * @return See eGrpStat.
     */
    static int grpActive(int gssi);

    /**
     * Adds an ISSI to a group's unconfirmed attachments, and removes from other
     * groups. Does nothing if ISSI already attached to the group.
     *
     * @param[in] issi The ISSI.
     * @param[in] gssi The GSSI.
     * @return true if ISSI added.
     */
    static bool grpUncAttach(int issi, int gssi);

    /**
     * Removes an ISSI from unconfirmed attachments of a group or all groups, or
     * clears all unconfirmed attachments.
     *
     * @param[in] issi The ISSI. 0 to clear all (GSSI irrelevant).
     * @param[in] gssi The GSSI. 0 for all groups.
     * @param[in] haveLock true if caller holds the data lock.
     */
    static void grpUncDetach(int issi = 0, int gssi = 0, bool haveLock = false);

    /**
     * Checks for presence of unconfirmed group attachments.
     *
     * @return true if present.
     */
    static bool hasGrpUncAttach();

    /**
     * Gets the unconfirmed attachments of a group or all groups.
     * Output format:
     *     One group:  Lines of ISSI-ranges.
     *     All groups: Lines of "GSSI: <space-separated ISSI-ranges>".
     *
     * @param[in] gssi The GSSI. Omit for all groups.
     * @return Formatted list.
     */
    static std::string getGrpUncAttach(int gssi = 0);

    /**
     * Sets the valid client ID ranges for the current client in a multi-cluster
     * system.
     *
     * @param[in] type   Client type - MsgSp::Value::SUBS_TYPE_DISPATCHER/MOBILE.
     * @param[in] ranges Space-separated ID ranges received from server in the
     *                   format as expected by parseBranchRanges().
     */
    static void setClientRanges(int type, const std::string &ranges);

    /**
     * Checks whether a client ID is valid, i.e. within the ranges set through
     * setClientRanges().
     * In a single-cluster system, this always returns true.
     *
     * @param[in] type Client type - MsgSp::Value::SUBS_TYPE_DISPATCHER/MOBILE.
     * @param[in] id   The ID.
     * @return true if valid.
     */
    static bool isValidClient(int type, int id);

    /**
     * Gets branch IDs and names for user selection.
     *
     * @param[out] dataMap The data.
     * @return true if output not empty.
     */
    static bool getBranchData(BranchMapT &dataMap)
    {
        dataMap = sBranchMap;
        return !dataMap.empty();
    }

    static int getBranchCount() { return sBranchMap.size(); }

    static bool isMultiCluster() { return !sValidClients.empty(); }
#endif //SERVERAPP

    /**
     * Gets the complete Subscriber data and converts to string for display.
     * Output format:
     *  VPN(2): 1,2
     *  VPN-3 DIV(2): 21,30
     *  DIV-1 Fleet(2): 1,17
     *  Fleet-1 ISSI(6): 5025987-5025990,5026101,5206102
     *  Fleet-1 Group(12): 3204781,3204782,3204900-3204909
     *  Fleet-1 DGNA-Ind(1): 3204886
     *  Fleet-1 DGNA-Grp(1): 3205005
     *  GSSI-3204937 SSI(2): 3204249,3204250
     *
     * @return The Subscriber data.
     */
    static std::string getData();

    /**
     * Gets Subscriber data counts and converts to string for display.
     * Output format:
     *  VPN: 4
     *  DIV: 4
     *  Fleet: 16
     *  GSSI: 1000
     *  ISSI: 5000
     *  VPN-1 DIV: 14
     *  VPN-1 GSSI: 99
     *  DIV-2 Fleet: 14
     *  Fleet-3 ISSI: 89
     *  Fleet-3 GSSI: 100
     *  Fleet-3 Group: 70
     *  Fleet-3 DGNA-Ind: 19
     *  Fleet-3 DGNA-Grp: 10
     *  GSSI-3204888 SSI: 15
     *  GSSI-3204888 Attached-ISSI: 5
     *
     * @return The Subscriber data counts.
     */
    static std::string getCounts();

    /**
     * Adds a dispatcher or mobile client ID.
     *
     * @param[in] id       The client ID.
     * @param[in] domain   The VOIP domain.
     * @param[in] disp     true for dispatcher.
     * @param[in] haveLock true if caller holds the data lock.
     */
    static void addClient(int                id,
                          const std::string &domain,
                          bool               disp,
                          bool               haveLock = false);

    /**
     * Removes a dispatcher ID or mobile/resource ISSI.
     *
     * @param[in] id       Client ID or ISSI.
     * @param[in] disp     true for dispatcher.
     * @param[in] haveLock true if caller holds the data lock.
     */
    static void removeResource(int id, bool disp, bool haveLock = false);

    /**
     * Gets all dispatcher or mobile client IDs.
     *
     * @param[in] disp       true for dispatcher.
     * @param[in] inclHeader Include a header for display.
     * @param[in] sep        Separator between IDs.
     * @return The IDs.
     */
    static std::string getClientData(bool disp,
                                     bool inclHeader = false,
                                     char sep = ',');

    /**
     * Gets the VOIP domain of a dispatcher or mobile client.
     *
     * @param[in] id   The client ID.
     * @param[in] disp true for dispatcher.
     * @return The domain.
     */
    static std::string getClientDomain(int id, bool disp);

    /**
     * Checks whether an ID is for a mobile client.
     *
     * @param[in] id The client ID.
     * @return true if mobile.
     */
    static bool isMobile(int id);

    /**
     * Gets the name of a given ISSI.
     *
     * @param[in] issi The ISSI.
     * @param[in] type The terminal type. See eTerminalType. Omit if unknown.
     * @return The name, or empty string if Subscriber data not ready or ISSI
     *         not found.
     */
    static std::string getIssiName(int issi, int type = -1);

    /**
     * Checks whether an ISSI is valid (defined in network).
     * On server, if client ID specified, also checks that the ISSI and client
     * are in common cluster.
     *
     * @param[in] issi The ISSI.
     * @param[in] cid  The client ID, if any.
     * @param[in] disp true if cid is dispatcher, otherwise mobile.
     * @return true if valid.
     */
#ifdef SERVERAPP
    static bool validIssi(int issi, int cid = 0, bool disp = false);
#else
    static bool validIssi(int issi);
#endif

    /**
     * Checks whether a GSSI is defined in the network when the caller is not
     * holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @return true if valid.
     */
    static bool validGssi(int gssi);

    /**
     * Gets the group name of a GSSI.
     *
     * @param[in] gssi The GSSI.
     * @return The name, or empty string if Subscriber data not ready or GSSI
     *         not found.
     */
    static std::string getGrpName(int gssi);

    /**
     * Gets the group type of a GSSI.
     *
     * @param[in] gssi The GSSI.
     * @return MsgSp::Value::SUBS_TYPE_GRP/DGNA_IND/DGNA_GRP.
     */
    static int getGrpType(int gssi);

    /**
     * Gets members of a group.
     *
     * @param[in]  gssi    The GSSI.
     * @param[out] members The member SSIs.
     * @return true if successful.
     */
    static bool getGrpMembers(int gssi, IdSetT &ssis);

    /**
     * Gets members of a group in string format.
     *
     * @param[in] gssi The GSSI.
     * @return The comma-separated member SSIs.
     */
    static std::string getGrpMembers(int gssi);

    /**
     * Gets attached members of a group.
     *
     * @param[in]  gssi  The GSSI.
     * @param[out] issis The member ISSIs.
     * @param[in]  unc   (Client only) true to include unconfirmed attachments
     *                   (see SUBS_GSSI_ATTACH_LIST_UNC).
     * @return true if successful.
     */
    static bool getGrpAttachedMembers(int gssi, IdSetT &issis, bool unc = true);

    /**
     * Gets attached members of a group formatted as either:
     *  -with header:
     *   "Group <gssi> - attached members(<count>): <comma-separated ISSIs>"
     *  -multiline:
     *   "<newline-separated ISSIs>", or empty string if no ISSIs
     *  -with braces:
     *   "{ISSI1, ISSI2, ISSI3...}", or empty string if no ISSIs
     * On client, this does not include unconfirmed attachments.
     * See getGrpUncAttach() instead for that.
     *
     * @param[in] gssi      The GSSI.
     * @param[in] header    true to format with header.
     * @param[in] multiline true for multiline, otherwise with braces.
     *                      Not applicable if header is true.
     * @return The formatted list.
     */
    static std::string getGrpAttachedMembers(int  gssi,
                                             bool header,
                                             bool multiline = false);

    /**
     * Checks whether an ISSI is attached to any of the given groups.
     *
     * @param[in] issi  The ISSI.
     * @param[in] gssis The group GSSIs.
     * @return true if attached.
     */
    static bool isGrpAttachedMember(int issi, const IdSetT &gssis);

    /**
     * Gets all ISSIs in a fleet, or in the entire network if in network-mode.
     *
     * @param[in] mob   true for mobile ISSIs, otherwise subscriber ISSIs.
     * @param[in] fleet The Fleet ID. FLEET_NONE to use the configured fleet, or
     *                  in network-mode.
     * @param[in] sep   Separator between ISSIs.
     * @return The ISSIs.
     */
    static std::string getIssis(bool mob   = false,
                                int  fleet = FLEET_NONE,
                                char sep   = ',');

    /**
     * Gets all ISSIs in a fleet, or in the entire network if in network-mode.
     *
     * @param[out] issis The ISSI list.
     * @param[in]  mob   true for mobile ISSIs, otherwise subscriber ISSIs.
     * @param[in]  fleet The Fleet ID. FLEET_NONE to use the configured fleet,
     *                   or in network-mode.
     * @return true if successful.
     */
    static bool getIssis(IdSetT &issis,
                         bool    mob   = false,
                         int     fleet = FLEET_NONE);

    /**
     * Gets VPN of a GSSI when the caller is not holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @return The VPN ID. 0 if not found.
     */
    static int getGssiVpn(int gssi);

    static int getFleet() { return sFleet; }

    static void resetState() { sState = STATE_INIT; }

    /**
     * Sets the state to STATE_DOWNLOADING.
     *
     * @return true if state changed, false if it is already
     *         STATE_DOWNLOADING.
     */
    static bool setStateDownloading();

    static bool isValid() { return (sState != STATE_INVALID); }

    static bool isDownloading() { return (sState == STATE_DOWNLOADING); }

    static bool isReady() { return (sState == STATE_READY); }

private:
    //holds VPN and fleet for a particular ISSI in network mode
    struct IssiInfo
    {
        IssiInfo() {}
        IssiInfo(int v, int f) : vpn(v), fleet(f) {}

        int vpn;
        int fleet;
    };
    typedef std::map<int, IssiInfo> IssiInfoMapT;

    static int                 sState;
    static int                 sVpn;
    static int                 sFleet;
    static time_t              sTimestamp;  //last update time
    static Id2DomMapT          sDispatcherIds;
    static Id2DomMapT          sMobileIds;
    static IdMapT              sData;       //main data
    //groups by type in each VPN - static groups here are the empty ones
    static IdMapT              sVpnGrps;
    static Int2IdsMapT         sFleetGrps;  //static groups in each fleet
    //description of each GSSI
    static Ssi2DescMapT        sGssiDesc;
    //description of each ISSI by terminal type
    static IssiType2DescMapT   sTypeIssiDesc;
    static IssiInfoMapT        sIssiInfo;
    static std::queue<MsgSp *> sMonQueue;
    static PalLock::LockT      sDataLock;

    typedef std::map<int, int>         Int2IntMapT;
    //ID ranges: key = type, value = min:max
    typedef std::map<int, Int2IntMapT> RangeMapT;

#ifdef SERVERAPP
    typedef std::set<std::string> StrSetT;

    //see addBranch() for explanation of branches
    struct BranchData
    {
        int         cluster;    //peer branches have the same value
        std::string name;       //hexadecimal-encoded UTF8 Unicode
        RangeMapT   idMap;      //key = BRID_SSI/BRID_MOB
        IdSetT      cids;       //client IDs (BRID_DISP)
        //server user IDs (BRID_SVR) in multi-cluster system
        StrSetT     svrUsrs;

        //checks whether this branch has an ID of a particular type, or any type
        bool hasId(int type, int id) const
        {
            switch (type)
            {
                case BRID_DISP:
                    return (cids.count(id) != 0);
                case BRID_SVR:
                    break; //not applicable - ID is string
                default:
                    if (type < 0)
                    {
                        //check all types
                        if (cids.count(id) != 0)
                            return true;
                        for (const auto &it : idMap)
                        {
                            for (const auto &it2 : it.second)
                            {
                                if (id < it2.first)
                                    break;
                                if (id <= it2.second)
                                    return true;
                            }
                        }
                    }
                    else if (idMap.count(type) != 0)
                    {
                        for (const auto &it : idMap.at(type))
                        {
                            if (id < it.first)
                                break;
                            if (id <= it.second)
                                return true;
                        }
                    }
                    break;
            }
            return false;
        }

        //checks whether this branch has a server user ID
        bool hasId(const std::string &usr) const
        {
            return (svrUsrs.count(usr) != 0);
        }

        //gets the number of IDs of a particular type
        int idCount(int type) const
        {
            switch (type)
            {
                case BRID_DISP:
                    return cids.size();
                case BRID_SVR:
                    return svrUsrs.size();
                default:
                    if (idMap.count(type) != 0)
                    {
                        int n = 0;
                        for (const auto &it : idMap.at(type))
                        {
                            n += (it.second - it.first + 1);
                        }
                        return n;
                    }
                    break;
            }
            return 0;
        }

        //gets the client IDs - dispatcher and mobile
        void getClientIds(IdSetT &out) const
        {
            out = cids;
            if (idMap.count(BRID_MOB) != 0)
            {
                int i;
                for (const auto &it : idMap.at(BRID_MOB))
                {
                    for (i=it.first; i<=it.second; ++i)
                    {
                        out.insert(i);
                    }
                }
            }
        }
    };
    typedef std::map<int, BranchData> BranchMapT; //key is branch

    //key is branch - branch 0 is for default in case of no branch config,
    //value is emergency GSSI
    static Int2IntMapT sEmSsi;
    static Int2IntMapT sClusterCount; //branch count for each cluster

#else
    //key is MsgSp::Value::SUBS_TYPE_DISPATCHER/MOBILE
    static RangeMapT sValidClients;
#endif //SERVERAPP

    static BranchMapT sBranchMap;

    /**
     * On server, sets the data update timestamp to the current time, and
     * adds the timestamp to a message.
     * On client, saves the data update timestamp from a received server
     * message.
     * Caller must be holding sDataLock.
     *
     * @param[in] msg The message to update (server) or the received message
     *                (client), if any.
     */
    static void setTimestamp(MsgSp *msg = 0);

    /**
     * Processes MON-SUBS-* or MON-SSDGNA-* message to update the subscriber
     * data.
     * Caller must be holding sDataLock.
     *
     * @param[in] msg The message.
     */
    static void processMonMsg(MsgSp *msg);

    /**
     * Gets the total size of Subscriber data lists of a given type. E.g. the
     * total number of divisions across all VPNs, or the total number of
     * ISSIs across all fleets.
     * Caller must be holding sDataLock.
     *
     * @param[in]  type The type. See eSubsType.
     * @return The size.
     */
    static int getTotalSize(int type);

    /**
     * Gets VPN of a GSSI when the caller is holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @return The VPN ID. 0 if not found.
     */
    static int getGssiVpnHaveLock(int gssi);

    /**
     * Converts MsgSp::Value::SUBS_TYPE_* to MsgSp::Value::GRP_TYPE_*.
     *
     * @param[in] type The type.
     * @return The converted type.
     */
    static int toGrpType(int type);

    /**
     * Converts MsgSp::Value::GRP_TYPE_* to MsgSp::Value::SUBS_TYPE_*.
     *
     * @param[in] type The type.
     * @return The converted type.
     */
    static int fromGrpType(int type);

    /**
     * Sets a group name.
     * On server, this is a network name.
     * On client, an empty name causes the group name to be erased.
     * Caller must be holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @param[in] name The group name.
     * @return true if a local name does not exist. Always true on client.
     */
    static bool setGssiDesc(int gssi, const std::string &name);

    /**
     * Erases a group name.
     * On the server, proceeds only if the group has not been assigned a
     * local name.
     * Caller must be holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     */
    static void eraseGssiDesc(int gssi);

    /**
     * Checks whether a GSSI is defined in the network when the caller is
     * holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @return true if valid.
     */
    static bool validGssiHaveLock(int gssi);

    /**
     * Gets the fleet of an ISSI when the caller is holding sDataLock.
     *
     * @param[in] issi The ISSI.
     * @return The fleet ID. Negative if not found.
     */
    static int getIssiFleetHaveLock(int issi);

#ifdef SERVERAPP
    /**
     * Gets a group name. This is the local name if defined, otherwise the
     * network name.
     * Caller must be holding sDataLock.
     *
     * @param[in] gssi The GSSI.
     * @return The name, or empty string if not found.
     */
    static std::string getGssiDesc(int gssi);

    /**
     * Sets a subscriber name from the network.
     * If the ISSI does not yet have a name, sets it with the default type.
     * Caller must be holding sDataLock.
     *
     * @param[in] issi The ISSI.
     * @param[in] name The subscriber name.
     * @param[in] type Default terminal type - eTerminalType.
     * @return The terminal type if a local name does not exist.
     *         See eTerminalType. Otherwise -1.
     */
    static int setIssiDesc(int                issi,
                           const std::string &name,
                           int                type = TERMINALTYPE_DEFAULT);

    /**
     * Gets a subscriber name. This is the local name if defined, otherwise
     * the network name.
     * Caller must be holding sDataLock.
     *
     * @param[in] issi The ISSI.
     * @return The name, or empty string if not found.
     */
    static std::string getIssiDesc(int issi);

    /**
     * Removes a range of IDs from a data range map.
     *
     * @param[in]     min  Range start.
     * @param[in]     max  Range end.
     * @param[in,out] data The data map.
     */
    static void removeBranchRange(int min, int max, Int2IntMapT &data);

    /**
     * Collects all IDs from a data range map.
     *
     * @param[in]  data The data map.
     * @param[out] ids  The IDs.
     */
    static void branchRangeToIds(const Int2IntMapT &data, IdSetT &ids);

    /**
     * Gets the number of IDs in a branch.
     * Caller must be holding sDataLock.
     *
     * @param[in] branch The branch.
     * @param[in] type   ID type - eBranchIdType.
     * @return The number.
     */
    static int getBranchIdCount(int branch, int type);

    /**
     * Gets client/resource IDs of a particular type in a branch.
     * Caller must be holding sDataLock.
     *
     * @param[in] branch The branch.
     * @param[in] type   ID type - eBranchIdType.
     * @return Space-separated ID ranges: id1 id2-id3 id4-id5 ...
     */
    static std::string getBranchIdsHaveLock(int branch, int type);

    /**
     * Checks whether an ID is either:
     * -in any of the given branches, or
     * -linked to all branches, i.e. not in any branch in single-cluster system.
     * Caller must be holding sDataLock.
     *
     * @param[in] branches The branches.
     * @param[in] type     ID type - eBranchIdType.
     * @param[in] id       The ID.
     * @return true if ID found or linked to all branches.
     */
    static bool isInBranchHaveLock(const IdSetT &branches, int type, int id);

    /**
     * Finds an ID of a type in any or a particular branch.
     * Caller must be holding sDataLock.
     *
     * @param[in] type   ID type - eBranchIdType.
     * @param[in] id     The ID.
     * @param[in] branch The branch. Omit to find in all branches.
     * @return true if found.
     */
    static bool isInBranchHaveLock(int type, int id, int branch = 0);

#else
    /**
     * Sets a subscriber name and type.
     * Caller must be holding sDataLock.
     *
     * @param[in] issi         The ISSI.
     * @param[in] terminalType The terminal type. See eTerminalType.
     * @param[in] name         The subscriber name.
     * @param[in] overwrite    true to overwrite existing entry.
     */
    static void setIssiDesc(int                issi,
                            int                terminalType,
                            const std::string &name,
                            bool               overwrite = true);

    /**
     * This is an overloaded function to modify an existing entry when the
     * type is unknown. An empty name removes the ISSI.
     * Caller must be holding sDataLock.
     *
     * @param[in] issi The ISSI.
     * @param[in] name The subscriber name.
     */
    static void setIssiDesc(int issi, const std::string &name);

    /**
     * Gets groups of a particular type from a VPN.
     * Caller must be holding sDataLock.
     *
     * @param[in]  vpnGrps The VPN groups by type.
     * @param[in]  type    The group type.
     * @param[out] groups  The groups.
     */
    static void getVpnGroups(const Int2IdsMapT &vpnGrps,
                             int                type,
                             Ssi2DescMapT      &groups);

    /**
     * Gets the group names for a list of GSSIs.
     * Caller must be holding sDataLock.
     *
     * @param[in]  gssis  The GSSIs.
     * @param[out] groups The group names.
     */
    static void getGrpNames(const IdSetT &gssis, Ssi2DescMapT &groups);
#endif //SERVERAPP

    /**
     * Parses a string of space-separated ID ranges into a data range map.
     * A range can be a single value or "<start>-<end>":
     *     id1 id2-id3 id4-id5 ...
     * Map key is the range start, and the value is the range end.
     *
     * @param[in]  str  The input string.
     * @param[out] data The map.
     * @return true if output not empty.
     */
    static bool parseBranchRanges(const std::string &str, Int2IntMapT &data);

    /**
     * Adds a range of IDs into a data range map.
     *
     * @param[in]     min  Range start.
     * @param[in]     max  Range end.
     * @param[in,out] data The data map.
     */
    static void addBranchRange(int min, int max, Int2IntMapT &data);
};
#endif //SUBSDATA_H
