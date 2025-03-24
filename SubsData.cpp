/**
 * Subscriber data manager implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: SubsData.cpp 1908 2025-03-05 00:54:00Z rosnin $
 * @author Zahari Hadzir
 */
#include <assert.h>

#ifdef SERVERAPP
#include "CfgManager.h"
#endif
#include "Locker.h"
#include "Utils.h"
#include "SubsData.h"

using namespace std;

int                         SubsData::sState(SubsData::STATE_INVALID);
int                         SubsData::sVpn(0);
int                         SubsData::sFleet(FLEET_NONE);
time_t                      SubsData::sTimestamp(0);
SubsData::Id2DomMapT        SubsData::sDispatcherIds;
SubsData::Id2DomMapT        SubsData::sMobileIds;
SubsData::IdMapT            SubsData::sData;
SubsData::IdMapT            SubsData::sVpnGrps;
SubsData::Int2IdsMapT       SubsData::sFleetGrps;
SubsData::Ssi2DescMapT      SubsData::sGssiDesc;
SubsData::IssiType2DescMapT SubsData::sTypeIssiDesc;
SubsData::IssiInfoMapT      SubsData::sIssiInfo;
queue<MsgSp *>              SubsData::sMonQueue;
#ifdef _WIN32
PalLock::LockT              SubsData::sDataLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT              SubsData::sDataLock(QMutex::Recursive);
#else
PalLock::LockT              SubsData::sDataLock = PTHREAD_MUTEX_INITIALIZER;
#endif
#ifdef SERVERAPP
SubsData::Int2IntMapT       SubsData::sEmSsi;
SubsData::Int2IntMapT       SubsData::sClusterCount;
#else
SubsData::RangeMapT         SubsData::sValidClients;
#endif
SubsData::BranchMapT        SubsData::sBranchMap;

void SubsData::final()
{
    while (!sMonQueue.empty())
    {
        delete sMonQueue.front();
        sMonQueue.pop();
    }
}

time_t SubsData::getTimestamp()
{
    Locker lock(&sDataLock);
    return sTimestamp;
}

string SubsData::getTimestampStr(bool showRaw)
{
    Locker lock(&sDataLock);
    if (sTimestamp == 0)
        return "0";
    string str(ctime(&sTimestamp));
    if (!showRaw)
        return str;
    ostringstream oss;
    oss << sTimestamp << ' ' << str;
    return oss.str();
}

bool SubsData::processMsg(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in SubsData::processMsg" == 0);
        return true;
    }
    bool   retVal = true;
    int    val;
    string valStr;
    Locker lock(&sDataLock);
    switch (msg->getType())
    {
        case MsgSp::Type::MON_LOC:
        {
            processMonMsg(msg);
            break;
        }
        case MsgSp::Type::MON_START:
        case MsgSp::Type::MON_STOP:
        {
            //only from STM and mobile clients - mobile grp attach/detach
            processMonMsg(msg);
            break;
        }
        case MsgSp::Type::MON_SUBS_DEFINE:
        case MsgSp::Type::MON_SUBS_DELETE:
        {
            if (!isDownloading() &&
                msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                    MsgSp::Value::SUBS_TYPE_DISPATCHER)
            {
                processMonMsg(msg);
                break;
            }
            //fallthrough
        }
        case MsgSp::Type::MON_GRP_ATTACH_DETACH:
        case MsgSp::Type::MON_SSDGNA_ASSIGN:
        case MsgSp::Type::MON_SSDGNA_DEASSIGN:
        case MsgSp::Type::MON_SSDGNA_DEFINE:
        case MsgSp::Type::MON_SSDGNA_DELETE:
        case MsgSp::Type::MON_SUBS_ASSIGN:
        case MsgSp::Type::MON_SUBS_DEASSIGN:
        case MsgSp::Type::MON_SUBS_PERMISSION:
        {
            if (isDownloading())
            {
                //queue a copy for later processing
                sMonQueue.push(new MsgSp(*msg));
                retVal = false;
            }
            else
            {
                processMonMsg(msg);
#ifdef MOBILE_CLIENTS
                //nwk mobiles are irrelevant to clients - discard msg
                if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                    MsgSp::Value::SUBS_TYPE_MOBILE)
                    retVal = false;
                else
#endif
                    setTimestamp(msg);
            }
            break;
        }
        case MsgSp::Type::MON_SUBS_DESC:
        {
            if (isDownloading())
            {
                //queue a copy for later processing
                sMonQueue.push(new MsgSp(*msg));
                retVal = false;
            }
            else
            {
                processMonMsg(msg);
                if (!msg->hasField(MsgSp::Field::DESC))
                    retVal = false; //name not used - discard msg
#ifdef MOBILE_CLIENTS
                //nwk mobiles are irrelevant to clients - discard msg
                else if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                         MsgSp::Value::SUBS_TYPE_MOBILE)
                    retVal = false;
#endif
                else
                    setTimestamp(msg); //name used
            }
            break;
        }
        case MsgSp::Type::SSDGNA_ASSIGN:
        {
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::RESULT_SSI_ALREADY_ASSIGNED)
                sData[SUBS_GSSI_SSI_LIST][msg->getFieldInt(MsgSp::Field::GSSI)]
                    .insert(msg->getFieldInt(MsgSp::Field::AFFECTED_USER));
            break;
        }
        case MsgSp::Type::SSDGNA_DEASSIGN:
        {
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::RESULT_GSSI_NOT_ASSIGNED)
            {
                val = msg->getFieldInt(MsgSp::Field::GSSI);
                //remove the deassigned group member GSSI
                if (sData[SUBS_GSSI_SSI_LIST].count(val) != 0)
                    sData[SUBS_GSSI_SSI_LIST][val]
                        .erase(msg->getFieldInt(MsgSp::Field::AFFECTED_USER));
            }
            break;
        }
        case MsgSp::Type::SUBS_DATA:
        {
            //final message in subscriber data download;
            //first, process queued MON-SUBS-*/MON-SSDGNA-* messages;
            //if the result is unsuccessful, the queue may contain
            //MON_SUBS_DEFINE messages for dispatcher IDs
            MsgSp *monMsg;
            while (!sMonQueue.empty())
            {
                monMsg = sMonQueue.front();
                sMonQueue.pop();
                processMonMsg(monMsg);
                delete monMsg;
            }
            if (msg->isResultSuccessful())
            {
                if (sFleet != FLEET_NONE)
                {
                    //in fleet mode, remove unnecessary data
                    sData.erase(SUBS_VPN_DIV_LIST);
                    sData.erase(SUBS_DIV_FLEET_LIST);
                }
                setTimestamp(msg);
                sState = STATE_READY;
            }
            else
            {
                final();
                sState = STATE_INIT;
            }
            break;
        }
        case MsgSp::Type::SUBS_DATA_GROUP:
        {
            //on client and STM-nwk svr only
            val = msg->getFieldInt(MsgSp::Field::GSSI);
            valStr.assign(msg->getFieldString(MsgSp::Field::ISSI_LIST));
            if (!valStr.empty())
            {
                //msg contains list of attached members
#ifdef SERVERAPP
                Utils::fromStringWithRange(valStr,
                                           sData[SUBS_GSSI_ATTACH_LIST][val],
                                           MsgSp::Value::LIST_DELIMITER);
#else
                IdSetT issis;
                Utils::fromStringWithRange(valStr, issis,
                                           MsgSp::Value::LIST_DELIMITER);
                sData[SUBS_GSSI_ATTACH_LIST][val]
                    .insert(issis.begin(), issis.end());
                for (auto i : issis)
                {
                    grpUncDetach(i, 0, true);
                }
#endif
                break;
            }
            setGssiDesc(val, msg->getFieldString(MsgSp::Field::GRP_NAME));
            //add members
            valStr.assign(msg->getFieldString(MsgSp::Field::SSI_LIST));
            if (!valStr.empty())
                Utils::fromStringWithRange(valStr,
                                           sData[SUBS_GSSI_SSI_LIST][val],
                                           MsgSp::Value::LIST_DELIMITER);
            break;
        }
#ifndef SERVERAPP
        case MsgSp::Type::SUBS_DATA_DIV:
        {
            //on client only, and process in network-mode only
            if (sFleet == FLEET_NONE)
                Utils::fromStringWithRange(
                                  msg->getFieldString(MsgSp::Field::FLEET_LIST),
                                  sData[SUBS_DIV_FLEET_LIST][
                                      msg->getFieldInt(MsgSp::Field::DIVISION)],
                                  MsgSp::Value::LIST_DELIMITER);
            break;
        }
        case MsgSp::Type::SUBS_DATA_FLEET:
        {
            //on client only
            val = msg->getFieldInt(MsgSp::Field::FLEET);
            if (val == MsgSp::Value::UNDEFINED)
                val = sFleet; //fleet-mode
            int key = -1;
            switch (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE))
            {
                case MsgSp::Value::SUBS_TYPE_ISSI:
                {
                    key = SUBS_FLEET_ISSI_LIST;
                    //fallthrough
                }
                case MsgSp::Value::SUBS_TYPE_MOBILE:
                {
                    if (key < 0)
                        key = SUBS_FLEET_MOB_LIST;
                    //fleet ISSIs
                    IdSetT issis;
                    Utils::fromStringWithRange(
                                   msg->getFieldString(MsgSp::Field::ISSI_LIST),
                                   issis, MsgSp::Value::LIST_DELIMITER);
                    sData[key][val].insert(issis.begin(), issis.end());
                    if (msg->hasField(MsgSp::Field::DESC_LIST))
                    {
                        //now need list of individual ISSIs instead of ranges
                        val = msg->getFieldInt(MsgSp::Field::TERMINAL_TYPE);
                        if (val == MsgSp::Value::UNDEFINED)
                            val = TERMINALTYPE_DEFAULT;
                        Utils::fromString(
                                  Utils::toString(issis,
                                                  MsgSp::Value::LIST_DELIMITER),
                                  msg->getFieldString(MsgSp::Field::DESC_LIST),
                                  sTypeIssiDesc[val],
                                  MsgSp::Value::LIST_DELIMITER, true);
                    }
                    break;
                }
                default:
                {
                    //fleet static groups
                    Utils::fromStringWithRange(
                                    msg->getFieldString(MsgSp::Field::GRP_LIST),
                                    sFleetGrps[val],
                                    MsgSp::Value::LIST_DELIMITER);
                    break;
                }
            }
            break;
        }
        case MsgSp::Type::SUBS_DATA_VPN:
        {
            //on client only
            val = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
            if (val != MsgSp::Value::UNDEFINED)
            {
                //add VPN groups
                Utils::fromStringWithRange(
                             msg->getFieldString(MsgSp::Field::GRP_LIST),
                             sVpnGrps[msg->getFieldInt(MsgSp::Field::VPN)][val],
                             MsgSp::Value::LIST_DELIMITER);
            }
            else if (sFleet == FLEET_NONE)
            {
                //add VPN divisions in network-mode only
                Utils::fromStringWithRange(
                  msg->getFieldString(MsgSp::Field::DIVISION_LIST),
                  sData[SUBS_VPN_DIV_LIST][msg->getFieldInt(MsgSp::Field::VPN)],
                  MsgSp::Value::LIST_DELIMITER);
            }
            break;
        }
#endif //!SERVERAPP
        default:
        {
            break;
        }
    }
    return retVal;
}

#ifdef SERVERAPP
void SubsData::init(int fleet)
{
    sFleet     = fleet;
    sState     = STATE_INIT;
    sVpn       = 0;
    sTimestamp = 0;
    sDispatcherIds.clear();
    sData.clear();
    sVpnGrps.clear();
    sFleetGrps.clear();
    sGssiDesc.clear();
    sTypeIssiDesc.clear();
    sIssiInfo.clear();
}

void SubsData::addList(int type, int key, const vector<int> &data)
{
    if (!data.empty())
    {
        Locker lock(&sDataLock);
        sData[type][key].insert(data.begin(), data.end());
    }
}

void SubsData::addFleetIssis(int                   vpn,
                             int                   fleet,
                             const vector<int>    &data,
                             const vector<int>    &types,
                             const vector<string> &desc)
{
    if (data.empty())
        return;
    Locker lock(&sDataLock);
    auto &issiList(sData[SUBS_FLEET_ISSI_LIST][fleet]);
    auto &mobList(sData[SUBS_FLEET_MOB_LIST][fleet]);
    auto it = data.begin();
    auto itEnd = data.end();
    for (auto tp : types)
    {
        if (tp == MsgSp::Value::ISSI_TYPE_MCS)
            mobList.insert(*it);
        else
            issiList.insert(*it);
        if (++it == itEnd)
            break;
    }
    //in case types not given or not enough
    for (; it!=itEnd; ++it)
    {
        issiList.insert(*it);
    }
    //descriptions are hexadecimal UCS2
    it = data.begin();
    for (auto &s : desc)
    {
        setIssiDesc(*it, Utils::hexUtf16ToUtf8(s),
           (mobList.count(*it) == 0)? TERMINALTYPE_DEFAULT: TERMINALTYPE_PHONE);
        if (++it == itEnd)
            break;
    }
    if (sFleet == FLEET_NONE)
    {
        //network-mode - store VPN and fleet of each ISSI
        for (auto i : data)
        {
            sIssiInfo[i] = IssiInfo(vpn, fleet);
        }
    }
}

bool SubsData::setFleetMob(bool add, int issi, int fleet)
{
    if (fleet < 0)
        fleet = sFleet;
    //mainly for web interface and STM-nwk
    Locker lock(&sDataLock);
    if (add)
    {
        sData[SUBS_FLEET_MOB_LIST][fleet].insert(issi);
        return true;
    }
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        if (fleet == FLEET_NONE)
        {
            //find in all fleets
            for (auto &it : sData[SUBS_FLEET_MOB_LIST])
            {
                if (it.second.erase(issi) != 0)
                    return true;
            }
        }
        else if (sData[SUBS_FLEET_MOB_LIST].count(fleet) != 0)
        {
            return (sData[SUBS_FLEET_MOB_LIST][fleet].erase(issi) != 0);
        }
    }
    return false;
}

bool SubsData::hasMobiles()
{
    Locker lock(&sDataLock);
    return (sData.count(SUBS_FLEET_MOB_LIST) != 0 &&
            !sData[SUBS_FLEET_MOB_LIST].empty());
}

void SubsData::addGrps(const vector<int>    &fleets,
                       const vector<int>    &gssis,
                       const vector<string> &names)
{
    assert(fleets.size() == gssis.size());
    Locker lock(&sDataLock);
    auto it = fleets.begin();
    auto itEnd = fleets.end();
    auto git = gssis.begin();
    for (auto &s : names)
    {
        if (sFleet == FLEET_NONE || sFleet == *it)
        {
            sFleetGrps[*it].insert(*git);
            setGssiDesc(*git, Utils::toHexString(s));
        }
        if (++it == itEnd)
            break;
        ++git;
    }
    //in case not enough names
    for (; it!=itEnd; ++it,++git)
    {
        if (sFleet == FLEET_NONE || sFleet == *it)
        {
            sFleetGrps[*it].insert(*git);
            setGssiDesc(*git, "");
        }
    }
}

void SubsData::addVpnGrps(int                   vpn,
                          int                   grpType,
                          const vector<int>    &gssis,
                          const vector<string> &names)
{
    if (gssis.empty())
        return;
    Locker lock(&sDataLock);
    auto it = gssis.begin();
    auto itEnd = gssis.end();
    sVpnGrps[vpn][fromGrpType(grpType)].insert(it, itEnd);
    //names are hexadecimal UCS2
    for (const auto &s : names)
    {
        setGssiDesc(*it, Utils::hexUtf16ToUtf8(s));
        if (++it == itEnd)
            break;
    }
    //in case not enough names
    for (; it!=itEnd; ++it)
    {
        setGssiDesc(*it, "");
    }
}

void SubsData::addGrpFleets(int gssi, const vector<int> &fleets)
{
    if (fleets.empty())
        return;
    Locker lock(&sDataLock);
    //add fleets to the GSSI (always a static group)
    sData[SUBS_GSSI_FLEET_LIST][gssi].insert(fleets.begin(), fleets.end());
    //move gssi from sVpnGrps to sFleetGrps because it is not empty
    int vpn = getGssiVpnHaveLock(gssi);
    if (vpn != 0 && sVpnGrps.count(vpn) != 0 &&
        sVpnGrps[vpn].count(MsgSp::Value::SUBS_TYPE_GRP) != 0)
        sVpnGrps[vpn][MsgSp::Value::SUBS_TYPE_GRP].erase(gssi);
    //add GSSI to the fleets
    if (sFleet == FLEET_NONE) //network-mode: always add
    {
        for (auto i : fleets)
        {
            sFleetGrps[i].insert(gssi);
        }
    }
    else            //fleet-mode: only for matching fleet
    {
        for (auto i : fleets)
        {
            if (sFleet == i)
            {
                sFleetGrps[i].insert(gssi);
                break;
            }
        }
    }
}

void SubsData::addGrpIssis(int gssi, int grpType, const vector<int> &issis)
{
    if (issis.empty())
        return;
    Locker lock(&sDataLock);
    sData[SUBS_GSSI_SSI_LIST][gssi].insert(issis.begin(), issis.end());
    //finished if gssi is not a static group
    if (grpType != MsgSp::Value::GRP_TYPE_STATIC)
        return;

    grpType = fromGrpType(grpType);
    //remove static group from sVpnGrps because it is not empty and thus
    //definitely can be assigned to a fleet
    int vpn = getGssiVpnHaveLock(gssi);
    if (vpn != 0 && sVpnGrps.count(vpn) != 0 &&
        sVpnGrps[vpn].count(grpType) != 0)
        sVpnGrps[vpn][grpType].erase(gssi);

    Int2IdsMapT &fleetIssisMap = sData[SUBS_FLEET_ISSI_LIST];
    if (sFleet != FLEET_NONE)
    {
        //find a member ISSI that belongs to the fleet
        for (auto i : issis)
        {
            if (fleetIssisMap[sFleet].count(i) != 0)
            {
                //add the group to the fleet
                sFleetGrps[sFleet].insert(gssi);
                break;
            }
        }
    }
    else
    {
        //check the fleet of each member ISSI to add this GSSI to that fleet
        for (const auto &it : fleetIssisMap)
        {
            //find a member ISSI that belongs to the current fleet
            for (auto i : issis)
            {
                if (it.second.count(i) != 0)
                {
                    //add the group to the fleet
                    sFleetGrps[it.first].insert(gssi);
                    break; //continue with the next fleet
                }
            }
        }
    }
}

void SubsData::addGrpGssis(int gssi, const vector<int> &gssis)
{
    if (!gssis.empty())
    {
        Locker lock(&sDataLock);
        sData[SUBS_GSSI_SSI_LIST][gssi].insert(gssis.begin(), gssis.end());
    }
}

bool SubsData::setGssiDescLocal(bool          updateTimestamp,
                                int           gssi,
                                const string &name,
                                bool          isHex)
{
    Locker lock(&sDataLock);
    string newName((name.empty() || isHex)? name: Utils::toHexString(name));
    if (sGssiDesc.count(gssi) == 0 || sGssiDesc[gssi].locName != newName)
    {
        sGssiDesc[gssi].locName = newName;
        if (updateTimestamp &&
            (!sGssiDesc[gssi].nwkName.empty() || validGssiHaveLock(gssi)))
        {
            //modification of valid network GSSI
            setTimestamp();
            return true;
        }
    }
    return false;
}

bool SubsData::setIssiDescLocal(bool          updateTimestamp,
                                int           type,
                                int           issi,
                                const string &name,
                                bool          isHex)
{
    Locker lock(&sDataLock);
    //find existing issi entry, which may be in another type
    auto it = sTypeIssiDesc.begin();
    for (; it!=sTypeIssiDesc.end(); ++it)
    {
        if (it->second.count(issi) != 0)
            break;
    }
    string newName((name.empty() || isHex)? name: Utils::toHexString(name));
    if (it == sTypeIssiDesc.end() || it->first != type ||
        it->second[issi].locName != newName)
    {
        sTypeIssiDesc[type][issi].locName = newName;
        if (it != sTypeIssiDesc.end() && it->first != type)
        {
            //moved to different type - copy network name
            sTypeIssiDesc[type][issi].nwkName = it->second[issi].nwkName;
            it->second.erase(issi);
        }
        if (updateTimestamp && getIssiFleetHaveLock(issi) >= 0)
        {
            //modification of valid network ISSI
            setTimestamp();
            return true;
        }
    }
    return false;
}

string SubsData::getGssiDescNwk(int gssi)
{
    Locker lock(&sDataLock);
    if (sGssiDesc.count(gssi) != 0)
        return sGssiDesc[gssi].nwkName;
    return "";
}

string SubsData::getIssiDescNwk(int type, int issi)
{
    Locker lock(&sDataLock);
    if (sTypeIssiDesc.count(type) != 0 &&
        sTypeIssiDesc[type].count(issi) != 0)
        return sTypeIssiDesc[type][issi].nwkName;
    //check other types
    for (auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
            return it.second[issi].nwkName;
    }
    return "";
}

int SubsData::setVpn(int div)
{
    Locker lock(&sDataLock);
    for (const auto &it : sData[SUBS_VPN_DIV_LIST])
    {
        if (it.second.count(div) != 0)
        {
            sVpn = it.first;
            break;
        }
    }
    return sVpn;
}

void SubsData::getVpns(IdSetT &data)
{
    Locker lock(&sDataLock);
    for (const auto &it : sData[SUBS_VPN_DIV_LIST])
    {
         data.insert(it.first);
    }
}

bool SubsData::getIssiVpnFleet(int issi, int &vpn, int &fleet)
{
    if (!isReady())
        return false;
    if (sFleet != FLEET_NONE) //fleet-mode
    {
        fleet = sFleet;
        vpn = sVpn;
    }
    else //network-mode
    {
        Locker lock(&sDataLock);
        if (sIssiInfo.count(issi) == 0)
            return false;
        vpn = sIssiInfo[issi].vpn;
        fleet = sIssiInfo[issi].fleet;
    }
    return true;
}

bool SubsData::getDataSet(int type, int key, IdSetT &data)
{
    Locker lock(&sDataLock);
    if (sData.count(type) == 0 || sData[type].count(key) == 0)
        return false;
    data = sData[type][key];
    return true;
}

bool SubsData::getData(IdMapT       &subsData,
                       IdMapT       &vpnGroups,
                       Int2IdsMapT  &fleetGroups,
                       Ssi2DescMapT &ssiDesc,
                       int           cid)
{
    if (!isReady())
        return false;
    IdSetT branches;
    if (!getIdBranches(cid, branches, BRID_DISP))
    {
        //cid not in specific branch, with 0 or 1 cluster - return all
        Locker lock(&sDataLock);
        subsData    = sData;
        vpnGroups   = sVpnGrps;
        fleetGroups = sFleetGrps;
        ssiDesc     = sGssiDesc;
        return true;
    }
    //only in fleet mode
    subsData.clear();
    vpnGroups.clear();
    fleetGroups.clear();
    if (branches.empty())
    {
        //cid not in any branch, with multiple clusters - no data
        ssiDesc.clear();
        return true;
    }
    getGrpDesc(ssiDesc, cid, true);
    Locker lock(&sDataLock);
    //iterate eSubsType in sData
    for (const auto &it : sData)
    {
        auto &outMap = subsData[it.first];
        switch (it.first)
        {
            case SUBS_FLEET_ISSI_LIST:
            {
                //iterate fleets
                for (const auto &it2 : it.second)
                {
                    auto &outList = outMap[it2.first];
                    //iterate issis and get those linked to cid
                    for (auto i : it2.second)
                    {
                        if (isInBranchHaveLock(branches, BRID_SSI, i))
                            outList.insert(i);
                    }
                }
                break;
            }
            case SUBS_FLEET_MOB_LIST:
            {
                //iterate fleets
                for (const auto &it2 : it.second)
                {
                    auto &outList = outMap[it2.first];
                    //iterate issis and get those linked to cid
                    for (auto i : it2.second)
                    {
                        if (isInBranchHaveLock(branches, BRID_MOB, i))
                            outList.insert(i);
                    }
                }
                break;
            }
            case SUBS_GSSI_ATTACH_LIST:
            case SUBS_GSSI_SSI_LIST:
            {
                //iterate gssis and get those linked to cid
                for (const auto &it2 : it.second)
                {
                    if (isInBranchHaveLock(branches, BRID_SSI, it2.first))
                        outMap[it2.first].insert(it2.second.begin(),
                                                 it2.second.end());
                }
                break;
            }
            default:
            {
                break; //do nothing - should not occur
            }
        }
    }
    //iterate vpns for groups
    for (const auto &it : sVpnGrps)
    {
        auto &outGrpTypes = vpnGroups[it.first];
        //iterate grp types
        for (const auto &it2 : it.second)
        {
            auto &outGssis = outGrpTypes[it2.first];
            //iterate gssis and get those linked to cid
            for (auto g : it2.second)
            {
                if (isInBranchHaveLock(branches, BRID_SSI, g))
                    outGssis.insert(g);
            }
        }
    }
    //iterate fleets for groups
    for (const auto &it : sFleetGrps)
    {
        auto &outGssis = fleetGroups[it.first];
        //iterate gssis and get those linked to cid
        for (auto g : it.second)
        {
            if (isInBranchHaveLock(branches, BRID_SSI, g))
                outGssis.insert(g);
        }
    }
    return true;
}

bool SubsData::getGrpDesc(Ssi2DescMapT &desc, int cid, bool dispReq)
{
    if (!isReady())
        return false;
    IdSetT branches;
    if (!getIdBranches(cid, branches, (dispReq)? BRID_DISP: BRID_MOB))
    {
        //cid not in specific branch, with 0 or 1 cluster - return all
        Locker lock(&sDataLock);
        desc = sGssiDesc;
    }
    else
    {
        desc.clear();
        if (!branches.empty())
        {
            //get groups linked to cid
            Locker lock(&sDataLock);
            for (const auto &it : sGssiDesc)
            {
                if (isInBranchHaveLock(branches, BRID_SSI, it.first))
                    desc[it.first] = it.second;
            }
        }
    }
    return true;
}

bool SubsData::getClientData(bool disp, int cid, bool dispReq, Id2DomMapT &ids)
{
    IdSetT branches;
    if (!getIdBranches(cid, branches, (dispReq)? BRID_DISP: BRID_MOB))
    {
        //cid not in specific branch, with 0 or 1 cluster - return all
        Locker lock(&sDataLock);
        ids = (disp)? sDispatcherIds: sMobileIds;
    }
    else
    {
        ids.clear();
        if (!branches.empty())
        {
            //get clients linked to cid
            int type = (disp)? BRID_DISP: BRID_MOB;
            Locker lock(&sDataLock);
            for (const auto &it : (disp)? sDispatcherIds: sMobileIds)
            {
                if (isInBranchHaveLock(branches, type, it.first))
                    ids[it.first] = it.second;
            }
        }
    }
    return !ids.empty();
}

void SubsData::getTerminalTypes(IdSetT &typeList)
{
    Locker lock(&sDataLock);
    for (const auto &it : sTypeIssiDesc)
    {
        typeList.insert(it.first);
    }
}

bool SubsData::filterIssis(int type, const IdSetT &inList, IdSetT &outList)
{
    outList.clear();
    Locker lock(&sDataLock);
    const Ssi2DescMapT &descMap(sTypeIssiDesc[type]);
    for (auto i : inList)
    {
        if (descMap.count(i) != 0)
            outList.insert(i);
    }
    return !outList.empty();
}

bool SubsData::isListEmpty(int type, int key)
{
    Locker lock(&sDataLock);
    return (sData[type].count(key) == 0 || sData[type][key].empty());
}

int SubsData::getIssiFleet(int issi)
{
    if (!isReady())
        return -1;
    Locker lock(&sDataLock);
    return getIssiFleetHaveLock(issi);
}

bool SubsData::isGssiInFleet(int fleet, int gssi)
{
    Locker lock(&sDataLock);
    return (sFleetGrps.count(fleet) != 0 && sFleetGrps[fleet].count(gssi) != 0);
}

bool SubsData::addBranch(int branch, int cluster, const string &name)
{
    if (branch <= 0 || name.empty())
        return false;
    Locker lock(&sDataLock);
    if (sBranchMap.count(branch) != 0)
        return false;
    //ensure unique name
    string hexName(Utils::toHexString(name));
    for (const auto &it : sBranchMap)
    {
        if (it.second.name == hexName)
            return false;
    }
    sBranchMap[branch].cluster = cluster;
    sBranchMap[branch].name = hexName;
    ++sClusterCount[cluster];
    return true;
}

bool SubsData::editBranch(int           branch,
                          int           cluster,
                          const string &name,
                          IdSetT       &affCids)
{
    if (cluster < 0 && name.empty())
        return false; //nothing to change
    Locker lock(&sDataLock);
    if (sBranchMap.count(branch) == 0)
        return false;
    auto &brData(sBranchMap[branch]);
    if (cluster >= 0)
    {
        int c = brData.cluster;
        if (cluster != c)
        {
            //changing cluster
            if (--sClusterCount[c] <= 0)
                sClusterCount.erase(c);
            brData.cluster = cluster;
            ++sClusterCount[cluster];
        }
    }
    string hexName(Utils::toHexString(name));
    if (!hexName.empty() && hexName != brData.name)
    {
        //ensure unique name
        for (const auto &it : sBranchMap)
        {
            if (it.first != branch && it.second.name == hexName)
                return false;
        }
        brData.name = hexName;
    }
    brData.getClientIds(affCids);
    setTimestamp();
    return true;
}

void SubsData::removeBranch(int branch, IdSetT &affCids)
{
    Locker lock(&sDataLock);
    if (sBranchMap.count(branch) != 0)
    {
        sBranchMap[branch].getClientIds(affCids);
        int c = sBranchMap[branch].cluster;
        sBranchMap.erase(branch);
        if (--sClusterCount[c] <= 0)
            sClusterCount.erase(c);
        setTimestamp();
    }
}

bool SubsData::getBranches(IdSetT &branches)
{
    Locker lock(&sDataLock);
    branches.clear();
    for (const auto &it : sBranchMap)
    {
        branches.insert(it.first);
    }
    return !branches.empty();
}

string SubsData::getBranches()
{
    ostringstream oss;
    Locker lock(&sDataLock);
    if (sClusterCount.size() <= 1)
    {
        for (const auto &it : sBranchMap)
        {
            oss << it.first << ' ' << Utils::fromHexString(it.second.name)
                << '\n';
        }
    }
    else
    {
        for (const auto &it : sBranchMap)
        {
            oss << it.first << ' ' << Utils::fromHexString(it.second.name)
                << ' ' << it.second.cluster << '\n';
        }
    }
    return oss.str();
}

int SubsData::getBranchInfo(int branch, string &name)
{
    Locker lock(&sDataLock);
    if (sBranchMap.count(branch) == 0)
        return -1;
    name = Utils::fromHexString(sBranchMap[branch].name);
    return sBranchMap[branch].cluster;
}

bool SubsData::addBranchIds(int           branch,
                            int           type,
                            const string &idList,
                            IdSetT       &affCids)
{
    if (type < BRID_MIN || type >= BRID_INVALID || idList.empty())
        return false;
    Locker lock(&sDataLock);
    switch (type)
    {
        case BRID_DISP:
        {
            IdSetT ids;
            if (Utils::fromStringWithRange(idList, ids, ' ') == 0)
                return false; //invalid input
            affCids.clear();
            //clients affected only if SSIs defined
            bool hasSsi = (sBranchMap[branch].idMap.count(BRID_SSI) != 0);
            auto &cids(sBranchMap[branch].cids);
            for (auto id : ids)
            {
                if (cids.count(id) == 0)
                {
                    cids.insert(id);
                    if (hasSsi)
                        affCids.insert(id);
                }
            }
            if (!affCids.empty())
                setTimestamp();
            break;
        }
        case BRID_MOB:
        {
            Int2IntMapT inp;
            if (!parseBranchRanges(idList, inp))
                return false; //invalid input
            affCids.clear();
            auto &brData(sBranchMap[branch]);
            IdSetT ids;
            branchRangeToIds(inp, ids);
            //affected IDs are those actually added
            for (auto id : ids)
            {
                if (!brData.hasId(type, id))
                    affCids.insert(id);
            }
            if (!affCids.empty())
            {
                auto &ranges = brData.idMap[type];
                for (auto &it : inp)
                {
                    addBranchRange(it.first, it.second, ranges);
                }
                setTimestamp();
            }
            break;
        }
        case BRID_SSI:
        {
            Int2IntMapT inp;
            if (!parseBranchRanges(idList, inp))
                return false; //invalid input
            affCids.clear();
            auto &brData(sBranchMap[branch]);
            int n = brData.idCount(type); //initial size
            auto &ranges = brData.idMap[type];
            for (auto &it : inp)
            {
                addBranchRange(it.first, it.second, ranges);
            }
            if (brData.idCount(type) > n) //really added
            {
                brData.getClientIds(affCids); //all clients
                setTimestamp();
            }
            break;
        }
        case BRID_SVR:
        default:
        {
            StrSetT ids;
            if (Utils::fromString(idList, ids, ' ') == 0)
                return false; //invalid input
            sBranchMap[branch].svrUsrs.insert(ids.begin(), ids.end());
            type = sBranchMap[branch].cluster; //reuse var
            //usr may only be in one cluster - remove from others
            for (auto &id : ids)
            {
                for (auto &it : sBranchMap)
                {
                    if (it.second.cluster != type)
                        it.second.svrUsrs.erase(id);
                }
            }
            break;
        }
    } //switch (type)
    return true;
}

void SubsData::removeBranchIds(int           branch,
                               int           type,
                               const string &idList,
                               IdSetT       &affCids)
{
    Locker lock(&sDataLock);
    if (sBranchMap.count(branch) == 0)
        return;
    auto &brData(sBranchMap[branch]);
    affCids.clear();
    switch (type)
    {
        case BRID_DISP:
        {
            if (idList.empty())
            {
                //all clients affected if SSIs defined
                if (brData.idMap.count(BRID_SSI) != 0)
                    affCids = brData.cids;
                brData.cids.clear(); //remove all
            }
            else
            {
                IdSetT ids;
                if (Utils::fromStringWithRange(idList, ids, ' ') == 0)
                    break;
                bool hasSsi = (brData.idMap.count(BRID_SSI) != 0);
                auto &cids = brData.cids;
                for (auto id : ids)
                {
                    if (cids.erase(id) != 0 && hasSsi)
                        affCids.insert(id);
                }
            }
            if (!affCids.empty())
                setTimestamp();
            break;
        }
        case BRID_MOB:
        {
            if (brData.idMap.count(type) == 0)
                break;
            Int2IntMapT inp;
            if (idList.empty())
            {
                branchRangeToIds(brData.idMap[type], affCids);
                brData.idMap.erase(type); //remove all
                setTimestamp();
            }
            else
            {
                if (!parseBranchRanges(idList, inp))
                    break;
                IdSetT ids;
                branchRangeToIds(inp, ids);
                //affected IDs are those that actually exist
                for (auto id : ids)
                {
                    if (brData.hasId(type, id))
                        affCids.insert(id);
                }
                if (!affCids.empty())
                {
                    auto &ranges = brData.idMap[type];
                    for (auto &it : inp)
                    {
                        removeBranchRange(it.first, it.second, ranges);
                    }
                    if (ranges.empty())
                        brData.idMap.erase(type);
                    setTimestamp();
                }
            }
            break;
        }
        case BRID_SSI:
        {
            if (brData.idMap.count(type) != 0)
            {
                if (idList.empty())
                {
                    brData.idMap.erase(type); //remove all
                    brData.getClientIds(affCids);
                    setTimestamp();
                }
                else
                {
                    Int2IntMapT inp;
                    if (!parseBranchRanges(idList, inp))
                        break;
                    int n = brData.idCount(type); //initial size
                    auto &ranges = brData.idMap[type];
                    for (auto &it : inp)
                    {
                        removeBranchRange(it.first, it.second, ranges);
                    }
                    if (ranges.empty())
                        brData.idMap.erase(type);
                    if (brData.idCount(type) < n) //really removed
                    {
                        brData.getClientIds(affCids);
                        setTimestamp();
                    }
                }
            }
            break;
        }
        case BRID_SVR:
        default:
        {
            if (idList.empty())
            {
                brData.svrUsrs.clear(); //remove all
            }
            else
            {
                StrSetT ids;
                if (Utils::fromString(idList, ids, ' ') != 0)
                {
                    auto &idSet = brData.svrUsrs;
                    for (auto id : ids)
                    {
                        idSet.erase(id);
                    }
                }
            }
            break;
        }
    } //switch (type)
}

string SubsData::getBranchIds(int branch, int type)
{
    Locker lock(&sDataLock);
    return getBranchIdsHaveLock(branch, type);
}

string SubsData::getBranchIds(const IdSetT &branches, int type)
{
    string s;
    Locker lock(&sDataLock);
    if (sClusterCount.size() > 1)
    {
        for (auto b : branches)
        {
            s.append(getBranchIdsHaveLock(b, type)).append(" ");
        }
    }
    return s;
}

bool SubsData::isInBranch(const IdSetT &branches, int type, int id)
{
    Locker lock(&sDataLock);
    return isInBranchHaveLock(branches, type, id);
}

bool SubsData::isInBranch(int type, int id, int branch)
{
    Locker lock(&sDataLock);
    return isInBranchHaveLock(type, id, branch);
}

bool SubsData::getIdBranches(int id, IdSetT &branches, int type)
{
    branches.clear();
    Locker lock(&sDataLock);
    if (sBranchMap.size() <= 1)
        return false; //no branching, or only one branch
    for (const auto &it : sBranchMap)
    {
        if (it.second.hasId(type, id))
            branches.insert(it.first);
    }
    //return false only if single-cluster and id not in any branch
    return (!branches.empty() || sClusterCount.size() > 1);
}

bool SubsData::getIdBranches(const string &usrId, IdSetT &branches, bool all)
{
    branches.clear();
    Locker lock(&sDataLock);
    if (sBranchMap.empty())
        return false;
    //if single-cluster, usr manages all
    if (all || sClusterCount.size() == 1)
    {
        for (const auto &it : sBranchMap)
        {
            branches.insert(it.first);
        }
        return (branches.size() > 1);
    }
    int cluster = -1; //find cluster
    for (const auto &it : sBranchMap)
    {
        if (it.second.hasId(usrId))
        {
            cluster = it.second.cluster;
            break;
        }
    }
    if (cluster >= 0) //otherwise unassigned in multi-cluster setup
    {
        for (const auto &it : sBranchMap)
        {
            if (it.second.cluster == cluster)
                branches.insert(it.first);
        }
    }
    return true;
}

string SubsData::getIdBranches(const string &idList, int type)
{
    IdSetT ids;
    if (Utils::fromStringWithRange(idList, ids, ' ') == 0)
        return "";
    ostringstream oss;
    if (type >= 0)
        oss << "Type " << type << '\n';
    int i = 0;
    Locker lock(&sDataLock);
    for (auto id : ids)
    {
        if (++i > 50)
        {
            oss << '(' << (ids.size() + 1 - i) << " more beyond limit - "
                << "rerun with range starting at " << id << ")\n";
            break;
        }
        oss << id << ':';
        for (const auto &it : sBranchMap)
        {
            if (it.second.hasId(type, id))
                oss << ' ' << it.first;
        }
        oss << '\n';
    }
    return oss.str();
}

bool SubsData::validSsi(int ssi, const string &usr)
{
    Locker lock(&sDataLock);
    if (sBranchMap.size() <= 1)
        return true;
    bool usrFound = false;
    bool ssiFound = false;
    for (const auto &it : sBranchMap)
    {
        if (it.second.hasId(usr))
        {
            if (it.second.hasId(BRID_SSI, ssi) ||
                it.second.hasId(BRID_MOB, ssi))
                return true;
            usrFound = true;
        }
        else if (!ssiFound &&
                 (it.second.hasId(BRID_SSI, ssi) ||
                  it.second.hasId(BRID_MOB, ssi)))
        {
            ssiFound = true;
        }
    }
    //in single-cluster setup, valid if either cid or ssi active in all branches
    return (sClusterCount.size() <= 1 && (!usrFound || !ssiFound));
}

string SubsData::getBranchUnassignedIds()
{
    IdSetT ids;
    ostringstream oss;
    Locker lock(&sDataLock);
    for (auto &it : sData[SUBS_FLEET_ISSI_LIST])
    {
        for (auto i : it.second)
        {
            if (!isInBranchHaveLock(BRID_SSI, i))
                ids.insert(i);
        }
    }
    if (!ids.empty())
    {
        oss << "ISSI(" << ids.size() << "): "
            << Utils::toStringWithRange(ids, " ") << "\n\n";
        ids.clear();
    }
    for (const auto &it : sVpnGrps)
    {
        for (const auto &it2 : it.second)
        {
            for (auto g : it2.second)
            {
                if (!isInBranchHaveLock(BRID_SSI, g))
                    ids.insert(g);
            }
        }
    }
    for (const auto &it : sFleetGrps)
    {
        for (auto g : it.second)
        {
            if (!isInBranchHaveLock(BRID_SSI, g))
                ids.insert(g);
        }
    }
    if (!ids.empty())
    {
        oss << "GSSI(" << ids.size() << "): "
            << Utils::toStringWithRange(ids, " ") << "\n\n";
        ids.clear();
    }
    for (auto &it : sDispatcherIds)
    {
        if (!isInBranchHaveLock(BRID_DISP, it.first))
            ids.insert(it.first);
    }
    if (!ids.empty())
        oss << "Disp(" << ids.size() << "): "
            << Utils::toStringWithRange(ids, " ") << "\n\n";
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        ids.clear();
        for (auto &it : sData[SUBS_FLEET_MOB_LIST])
        {
            for (auto i : it.second)
            {
                if (!isInBranchHaveLock(BRID_MOB, i))
                    ids.insert(i);
            }
        }
        if (!ids.empty())
            oss << "Mobile(" << ids.size() << "): "
                << Utils::toStringWithRange(ids, " ") << "\n\n";
    }
    return oss.str();
}

bool SubsData::getAllowedBranches(int cid, MsgSp &msg)
{
    Locker lock(&sDataLock);
    if (sBranchMap.size() <= 1)
        return false; //no branching or only single branch
    //get allowed clusters in multi-cluster config
    IdSetT cls;
    bool singleCls = (sClusterCount.size() <= 1);
    if (!singleCls)
    {
        for (const auto &it : sBranchMap)
        {
            if (it.second.hasId(BRID_DISP, cid))
                cls.insert(it.second.cluster);
        }
        if (cls.empty())
            return false; //unassigned cid in multi-cluster config
    }
    vector<int> brs;
    vector<string> nms;
    for (const auto &it : sBranchMap)
    {
        if (singleCls || cls.count(it.second.cluster) != 0)
        {
            brs.push_back(it.first);
            nms.push_back(it.second.name); //in hex
        }
    }
    //add to msg only if multiple
    if (brs.size() <= 1)
        return false;
    msg.addField(MsgSp::Field::GRP_LIST,
                 Utils::toString(brs, MsgSp::Value::LIST_DELIMITER));
    msg.addField(MsgSp::Field::DESC_LIST,
                 Utils::toString(nms, MsgSp::Value::LIST_DELIMITER));
    return true;
}

int SubsData::setIdBranches(int cid, const MsgSp &msg)
{
    IdSetT reqBrs; //requested branches
    string s(msg.getFieldString(MsgSp::Field::GRP_LIST));
    if (!s.empty())
        Utils::fromString(s, reqBrs, MsgSp::Value::LIST_DELIMITER);
    Locker lock(&sDataLock);
    bool singleCls = (sClusterCount.size() <= 1);
    //get allowed clusters in multi-cluster system, to validate reqBrs
    IdSetT cls;
    if (!singleCls)
    {
        for (const auto &it : sBranchMap)
        {
            if (it.second.hasId(BRID_DISP, cid))
                cls.insert(it.second.cluster);
        }
        if (cls.empty())
            return MsgSp::Value::RESULT_NOT_AUTHORIZED;
    }
    if (!reqBrs.empty())
    {
        //add cid to requested branches while filtering out invalid branches
        auto it = reqBrs.begin();
        while (it != reqBrs.end())
        {
            if (sBranchMap.count(*it) == 0)
            {
                it = reqBrs.erase(it);
            }
            else
            {
                if (singleCls || cls.count(sBranchMap[*it].cluster) != 0)
                    sBranchMap[*it].cids.insert(cid);
                ++it;
            }
        }
        if (reqBrs.empty())
            return MsgSp::Value::RESULT_REJECTED;
    }
    //request to:
    //-remove from all branches - cid becomes linked to all (single-cluster)
    //-add to all allowed branches (multi-cluster)
    else if (!singleCls)
    {
        for (auto &it : sBranchMap)
        {
            if (cls.count(it.second.cluster) != 0)
                it.second.cids.insert(cid);
        }
    }
    if (!reqBrs.empty() || singleCls)
    {
        //remove cid from other branches (all branches if reqBrs empty)
        for (auto &it : sBranchMap)
        {
            if (reqBrs.count(it.first) == 0)
                it.second.cids.erase(cid);
        }
    }
    setTimestamp();
    return MsgSp::Value::RESULT_OK;
}

void SubsData::setEmSsi(int ssi, int branch)
{
    if (ssi == 0)
        sEmSsi.erase(branch);
    else
        sEmSsi[branch] = ssi;
}

int SubsData::getEmSsi(int branch)
{
    if (sEmSsi.count(branch) != 0)
        return sEmSsi[branch];
    return 0;
}

int SubsData::getClientEmSsi(int id)
{
    if (sEmSsi.empty())
        return 0;
    int br = sEmSsi.begin()->first;
    if (sEmSsi.size() == 1 && br == 0)
        return sEmSsi.at(br); //no branch
    IdSetT brs;
    getIdBranches(id, brs, BRID_MOB);
    //error case of getIdBranches() returning true with empty branches is
    //already rejected on login,
    //if empty brs which means id active in all branches, get from the first
    //non-zero branch
    if (brs.empty())
    {
        for (auto &it : sEmSsi)
        {
            if (it.first != 0)
                return it.second;
        }
    }
    //get from first branch in brs that exists in sEmSsi
    for (auto &b : brs)
    {
        if (sEmSsi.count(b) != 0)
            return sEmSsi.at(b);
    }
    if (sEmSsi.count(0) != 0)
        return sEmSsi.at(0); //default
    return 0;
}

#else //SERVERAPP
void SubsData::clientInit(int fleet)
{
    sFleet = fleet;
    sState = STATE_INIT;
    sDispatcherIds.clear();
    sMobileIds.clear();
}

void SubsData::clientInit(MsgSp *msg)
{
    switch (msg->getType())
    {
        case MsgSp::Type::BRANCH_DATA:
        {
            if (msg->hasField(MsgSp::Field::GRP_LIST) &&
                msg->hasField(MsgSp::Field::DESC_LIST))
            {
                //save allowed branches from server
                sBranchMap.clear();
                vector<int> ids;
                vector<string> nms;
                msg->getFieldVals(MsgSp::Field::GRP_LIST, ids);
                msg->getFieldVals(MsgSp::Field::DESC_LIST, nms);
                auto nit = nms.begin();
                for (auto i : ids)
                {
                    if (nit == nms.end())
                    {
                        //should not occur
                        sBranchMap[i] = Utils::toHexString("<unnamed>");
                    }
                    else
                    {
                        sBranchMap[i] = *nit; //name in hex
                        ++nit;
                    }
                }
            }
            if (msg->hasField(MsgSp::Field::SSI_LIST))
                setClientRanges(MsgSp::Value::SUBS_TYPE_DISPATCHER,
                                msg->getFieldString(MsgSp::Field::SSI_LIST));
            if (msg->hasField(MsgSp::Field::ISSI_LIST))
                setClientRanges(MsgSp::Value::SUBS_TYPE_MOBILE,
                                msg->getFieldString(MsgSp::Field::ISSI_LIST));
            break;
        }
        case MsgSp::Type::SUBS_DATA:
        {
            sData.clear();
            sVpnGrps.clear();
            sFleetGrps.clear();
            sGssiDesc.clear();
            sTypeIssiDesc.clear();
            sIssiInfo.clear();
            break;
        }
        default:
        {
            break; //do nothing
        }
    }
}

bool SubsData::getGroups(int type, Ssi2DescMapT &groups)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    if (sGssiDesc.empty())
        return false;
    if (type == MsgSp::Value::SUBS_TYPE_GRP && !sFleetGrps.empty())
    {
        //loop through fleets for groups with assigned (current or previous)
        //members; in fleet-mode, sFleetGrps has only 1 fleet: sFleet
        for (const auto &it : sFleetGrps)
        {
            getGrpNames(it.second, groups);
        }
    }
    if (!sVpnGrps.empty())
    {
        //loop through VPNs for the given group type of each VPN
        for (const auto &it : sVpnGrps)
        {
            getVpnGroups(it.second, type, groups);
        }
    }
    return !groups.empty();
}

bool SubsData::getGroups(Ssi2DescMapT &stat,
                         Ssi2DescMapT &dgnaInd,
                         Ssi2DescMapT &dgnaGrp)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    if (sGssiDesc.empty())
        return false;
    //loop through fleets for groups with assigned (current or previous)
    //members; in fleet-mode, sFleetGrps has only 1 fleet: sFleet
    for (const auto &it : sFleetGrps)
    {
        getGrpNames(it.second, stat);
    }
    //loop through VPNs for all DGNA groups and empty static groups of each VPN
    for (const auto &it : sVpnGrps)
    {
        getVpnGroups(it.second, MsgSp::Value::SUBS_TYPE_GRP, stat);
        getVpnGroups(it.second, MsgSp::Value::SUBS_TYPE_DGNA_IND, dgnaInd);
        getVpnGroups(it.second, MsgSp::Value::SUBS_TYPE_DGNA_GRP, dgnaGrp);
    }
    return true;
}

bool SubsData::getDgnaAllowedIssis(int gssi, IdSetT &issis)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    //gssi must be a DGNA-Ind
    int vpn = getGssiVpnHaveLock(gssi);
    if (vpn == 0 || sVpnGrps.count(vpn) == 0 ||
        sVpnGrps[vpn][MsgSp::Value::SUBS_TYPE_DGNA_IND].count(gssi) == 0)
        return false;

    if (sFleet != FLEET_NONE) //fleet-mode
    {
        //get all fleet ISSIs
        if (sData[SUBS_FLEET_ISSI_LIST].count(sFleet) != 0)
            issis.insert(sData[SUBS_FLEET_ISSI_LIST][sFleet].begin(),
                         sData[SUBS_FLEET_ISSI_LIST][sFleet].end());
    }
    else //get the group VPN ISSIs
    {
        //iterate through the VPN's divisions
        for (auto div : sData[SUBS_VPN_DIV_LIST][vpn])
        {
            if (sData[SUBS_DIV_FLEET_LIST].count(div) == 0)
                continue;
            //iterate through each division's fleets
            for (auto fleet : sData[SUBS_DIV_FLEET_LIST][div])
            {
                //get ISSIs of the fleet
                if (sData[SUBS_FLEET_ISSI_LIST].count(fleet) != 0)
                    issis.insert(sData[SUBS_FLEET_ISSI_LIST][fleet].begin(),
                                 sData[SUBS_FLEET_ISSI_LIST][fleet].end());
            }
        }
    }
    return !issis.empty();
}

bool SubsData::getDgnaAllowedGssis(int gssi, IdSetT &gssis)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    //gssi must be a DGNA-Grp
    int vpn = getGssiVpnHaveLock(gssi);
    if (vpn == 0 || sVpnGrps.count(vpn) == 0 ||
        sVpnGrps[vpn][MsgSp::Value::SUBS_TYPE_DGNA_GRP].count(gssi) == 0)
        return false;

    if (sFleet != FLEET_NONE) //fleet-mode
    {
        //get the fleet's static groups
        getGssis(sFleetGrps[sFleet], gssis);
    }
    else if (sData[SUBS_VPN_DIV_LIST].count(vpn) != 0)
    {
        //get static groups of the VPN's fleets
        //iterate through the VPN's divisions
        for (auto div : sData[SUBS_VPN_DIV_LIST][vpn])
        {
            if (sData[SUBS_DIV_FLEET_LIST].count(div) == 0)
                continue;
            //iterate through each division's fleets
            for (auto fleet : sData[SUBS_DIV_FLEET_LIST][div])
            {
                if (sFleetGrps.count(fleet) != 0)
                    getGssis(sFleetGrps[fleet], gssis);
            }
        }
    } //else if (sData[SUBS_VPN_DIV_LIST].count(vpn) != 0)
    //get GSSIs of the VPN excluding DGNA-Grp
    Int2IdsMapT &vg(sVpnGrps[vpn]);
    if (vg.count(MsgSp::Value::SUBS_TYPE_DGNA_IND) != 0)
        getGssis(vg[MsgSp::Value::SUBS_TYPE_DGNA_IND], gssis);
    if (vg.count(MsgSp::Value::SUBS_TYPE_GRP) != 0)
        getGssis(vg[MsgSp::Value::SUBS_TYPE_GRP], gssis);
    return !gssis.empty();
}

void SubsData::getGssis(const IdSetT &src,
                        IdSetT       &gssis,
                        const IdSetT *excludes)
{
    if (excludes == 0)
    {
        gssis.insert(src.begin(), src.end());
    }
    else
    {
        for (auto i : src)
        {
            if (excludes->count(i) == 0)
                gssis.insert(i);
        }
    }
}

int SubsData::getIssiType(int issi)
{
    Locker lock(&sDataLock);
    for (const auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
            return it.first;
    }
    return -1;
}

int SubsData::grpActive(int gssi)
{
    if (isReady())
    {
        Locker lock(&sDataLock);
        if ((sData[SUBS_GSSI_ATTACH_LIST].count(gssi) != 0 &&
             !sData[SUBS_GSSI_ATTACH_LIST][gssi].empty()) ||
            (sData[SUBS_GSSI_ATTACH_LIST_UNC].count(gssi) != 0 &&
             !sData[SUBS_GSSI_ATTACH_LIST_UNC][gssi].empty()))
            return GRP_STAT_ATTACH;
        if (sData[SUBS_GSSI_SSI_LIST].count(gssi) != 0 &&
            !sData[SUBS_GSSI_SSI_LIST][gssi].empty())
            return GRP_STAT_ASSIGN;
    }
    return GRP_STAT_NONE;
}

bool SubsData::grpUncAttach(int issi, int gssi)
{
    if (issi <= 0 || gssi <= 0 || isGrpAttachedMember(issi, gssi))
        return false;
    Locker lock(&sDataLock);
    grpUncDetach(issi, 0, true); //detach from all first
    sData[SUBS_GSSI_ATTACH_LIST_UNC][gssi].insert(issi);
    return true;
}

void SubsData::grpUncDetach(int issi, int gssi, bool haveLock)
{
    if (!haveLock)
        PalLock::take(&sDataLock);
    auto &dm(sData[SUBS_GSSI_ATTACH_LIST_UNC]);
    if (issi == 0)
    {
        dm.clear();
    }
    else if (gssi == 0)
    {
        auto it = dm.begin();
        while (it != dm.end())
        {
            if (it->second.erase(issi) != 0 && it->second.empty())
                it = dm.erase(it);
            else
                ++it;
        }
    }
    else if (dm.count(gssi) != 0 && dm[gssi].erase(issi) != 0 &&
             dm[gssi].empty())
    {
        dm.erase(gssi);
    }
    if (!haveLock)
        PalLock::release(&sDataLock);
}

bool SubsData::hasGrpUncAttach()
{
    Locker lock(&sDataLock);
    return !sData[SUBS_GSSI_ATTACH_LIST_UNC].empty();
}

string SubsData::getGrpUncAttach(int gssi)
{
    Locker lock(&sDataLock);
    auto &dm(sData[SUBS_GSSI_ATTACH_LIST_UNC]);
    if (dm.empty() || (gssi != 0 && dm.count(gssi) == 0))
        return "";
    if (gssi != 0)
        return Utils::toStringWithRange(dm[gssi], "\n");
    ostringstream oss;
    for (const auto &it : dm)
    {
        oss << it.first << ": " << Utils::toStringWithRange(it.second, " ")
            << '\n';
    }
    return oss.str();
}

void SubsData::setClientRanges(int type, const string &ranges)
{
    if (type == MsgSp::Value::SUBS_TYPE_DISPATCHER ||
        type == MsgSp::Value::SUBS_TYPE_MOBILE)
    {
        Locker lock(&sDataLock);
        sValidClients[type].clear();
        parseBranchRanges(ranges, sValidClients[type]);
    }
}

bool SubsData::isValidClient(int type, int id)
{
    Locker lock(&sDataLock);
    if (sValidClients.empty())
        return true; //single cluster - no restriction
    if (sValidClients.count(type) != 0)
    {
        for (const auto &it : sValidClients.at(type))
        {
            if (id < it.first)
                break;
            if (id <= it.second)
                return true;
        }
    }
    return false;
}
#endif //SERVERAPP

string SubsData::getData()
{
    if (!isReady())
        return "";
    ostringstream oss;
    Locker lock(&sDataLock);
    size_t size = 0;
    if (!sData[SUBS_VPN_DIV_LIST].empty())
    {
        auto dit = sData[SUBS_VPN_DIV_LIST].begin();
        auto ditEnd = sData[SUBS_VPN_DIV_LIST].end();
        //VPNs
        oss << "VPN(" << sData[SUBS_VPN_DIV_LIST].size() << "): "
            << dit->first;
        for (++dit; dit!=ditEnd; ++dit)
        {
            oss << ',' << dit->first;
        }
        oss << '\n';
        //VPN-x divisions
        for (const auto &it : sData[SUBS_VPN_DIV_LIST])
        {
            size = it.second.size();
            if (size > 0)
                oss << "VPN-" << it.first << " DIV(" << size << "): "
                    << Utils::toStringWithRange(it.second) << '\n';
        }
    }
    //DIV-x fleets
    if (!sData[SUBS_DIV_FLEET_LIST].empty())
    {
        for (const auto &it : sData[SUBS_DIV_FLEET_LIST])
        {
            size = it.second.size();
            if (size > 0)
                oss << "DIV-" << it.first << " Fleet(" << size << "): "
                    << Utils::toStringWithRange(it.second) << '\n';
        }
    }
    //ISSIs and static groups of each fleet
    if (!sData[SUBS_FLEET_ISSI_LIST].empty())
    {
        for (const auto &it : sData[SUBS_FLEET_ISSI_LIST])
        {
            oss << "Fleet-" << it.first << " ISSI(" << it.second.size()
                << "): " << Utils::toStringWithRange(it.second) << '\n';
            if (sFleetGrps.count(it.first) == 0)
                continue;
            //static groups
            size = sFleetGrps[it.first].size();
            if (size > 0)
                oss << "Fleet-" << it.first << " Group(" << size << "): "
                    << Utils::toStringWithRange(sFleetGrps[it.first])
                    << '\n';
        }
    }
    //mobile ISSIs of each fleet
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        for (const auto &it : sData[SUBS_FLEET_MOB_LIST])
        {
            oss << "Fleet-" << it.first << " MobISSI(" << it.second.size()
                << "): " << Utils::toStringWithRange(it.second) << '\n';
        }
    }
    //fleets of each GSSI
    if (!sData[SUBS_GSSI_FLEET_LIST].empty())
    {
        for (const auto &it : sData[SUBS_GSSI_FLEET_LIST])
        {
            size = it.second.size();
            if (size > 0)
                oss << "GSSI-" << it.first << " Fleet(" << size << "): "
                    << Utils::toStringWithRange(it.second) << '\n';
        }
    }
    //SSIs of each GSSI
    if (!sData[SUBS_GSSI_SSI_LIST].empty())
    {
        for (const auto &it : sData[SUBS_GSSI_SSI_LIST])
        {
            size = it.second.size();
            if (size > 0)
                oss << "GSSI-" << it.first << " SSI(" << size << "): "
                    << Utils::toStringWithRange(it.second) << '\n';
        }
    }
    if (!sVpnGrps.empty())
    {
        oss << "Empty static groups, and DGNA groups:\n";
        for (auto &it : sVpnGrps)
        {
            if (it.second.count(MsgSp::Value::SUBS_TYPE_GRP) != 0)
            {
                oss << "VPN-" << it.first << " Group("
                    << it.second[MsgSp::Value::SUBS_TYPE_GRP].size()
                    << "): "
                    << Utils::toStringWithRange(
                                      it.second[MsgSp::Value::SUBS_TYPE_GRP])
                    << '\n';
            }
            if (it.second.count(MsgSp::Value::SUBS_TYPE_DGNA_IND) != 0)
            {
                oss << "VPN-" << it.first << " DGNA-Ind("
                    << it.second[MsgSp::Value::SUBS_TYPE_DGNA_IND].size()
                    << "): "
                    << Utils::toStringWithRange(
                                 it.second[MsgSp::Value::SUBS_TYPE_DGNA_IND])
                    << '\n';
            }
            if (it.second.count(MsgSp::Value::SUBS_TYPE_DGNA_GRP) != 0)
            {
                oss << "VPN-" << it.first << " DGNA-Grp("
                    << it.second[MsgSp::Value::SUBS_TYPE_DGNA_GRP].size()
                    << "): "
                    << Utils::toStringWithRange(
                                 it.second[MsgSp::Value::SUBS_TYPE_DGNA_GRP])
                    << '\n';
            }
        }
    }
#ifdef SERVERAPP
    //branch data
    for (const auto &it : sBranchMap)
    {
        oss << "Branch-" << it.first << ' '
            << Utils::fromHexString(it.second.name) << " Cluster-"
            << it.second.cluster << " Disp("
            << getBranchIdCount(it.first, BRID_DISP) << "): "
            << getBranchIdsHaveLock(it.first, BRID_DISP) << '\n';
        for (const auto &it2 : it.second.idMap)
        {
            if (it2.second.empty())
                continue;
            oss << "Branch-" << it.first << ' ';
            switch (it2.first)
            {
                case BRID_MOB:
                    oss << "Mobile(";
                    break;
                case BRID_SSI:
                    oss << "SSI(";
                    break;
                default:
                    break; //should not occur
            }
            oss << getBranchIdCount(it.first, it2.first) << "): "
                << getBranchIdsHaveLock(it.first, it2.first) << '\n';
        }
    }
#endif //SERVERAPP
    return oss.str();
}

string SubsData::getCounts()
{
    if (!isReady())
        return "";
    ostringstream oss;
    Locker lock(&sDataLock);
    //number of VPNs and divisions
    if (!sData[SUBS_VPN_DIV_LIST].empty())
    {
        oss << "VPN: " << sData[SUBS_VPN_DIV_LIST].size()
            << "\nDIV: " << getTotalSize(SUBS_VPN_DIV_LIST) << '\n';
    }
    //number of fleets
    if (!sData[SUBS_DIV_FLEET_LIST].empty())
        oss << "Fleet: " << getTotalSize(SUBS_DIV_FLEET_LIST) << '\n';
    //number of GSSIs
    size_t size = 0;
    int dgnaInd = 0;
    int dgnaGrp = 0;
    if (!sVpnGrps.empty())
    {
        //DGNA groups and empty static groups in all VPNs
        for (auto &it : sVpnGrps)
        {
            size += it.second[MsgSp::Value::SUBS_TYPE_GRP].size();
            dgnaInd += it.second[MsgSp::Value::SUBS_TYPE_DGNA_IND].size();
            dgnaGrp += it.second[MsgSp::Value::SUBS_TYPE_DGNA_GRP].size();
        }
    }
    IdSetT gssis;
    if (!sFleetGrps.empty())
    {
        //iterate through fleets
        for (const auto &it : sFleetGrps)
        {
            //a static group can be assigned to more than one fleet;
            //put them in a set to avoid duplication before getting the size
            gssis.insert(it.second.begin(), it.second.end());
        }
        size += gssis.size();
    }
    oss << "GSSI-Static: " << size
        << "\nGSSI-DGNA-Ind: " << dgnaInd
        << "\nGSSI-DGNA-Grp: " << dgnaGrp << '\n';
    //number of ISSIs
    if (!sData[SUBS_FLEET_ISSI_LIST].empty())
        oss << "ISSI: " << getTotalSize(SUBS_FLEET_ISSI_LIST) << '\n';
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0 &&
        !sData[SUBS_FLEET_MOB_LIST].empty())
        oss << "MobISSI: " << getTotalSize(SUBS_FLEET_MOB_LIST) << '\n';

    //content of each VPN
    for (const auto &it : sData[SUBS_VPN_DIV_LIST])
    {
        size = it.second.size();
        if (size > 0)
        {
            oss << "VPN-" << it.first << " DIV: " << size << '\n';
            size = 0;
        }
        dgnaInd = 0;
        dgnaGrp = 0;
        //groups that are not assigned to fleets
        if (sVpnGrps.count(it.first) != 0)
        {
            Int2IdsMapT &grpsMap = sVpnGrps[it.first];
            if (grpsMap.count(MsgSp::Value::SUBS_TYPE_GRP) != 0)
                size = grpsMap[MsgSp::Value::SUBS_TYPE_GRP].size();
            if (grpsMap.count(MsgSp::Value::SUBS_TYPE_DGNA_IND) != 0)
                dgnaInd = grpsMap[MsgSp::Value::SUBS_TYPE_DGNA_IND].size();
            if (grpsMap.count(MsgSp::Value::SUBS_TYPE_DGNA_GRP) != 0)
                dgnaGrp = grpsMap[MsgSp::Value::SUBS_TYPE_DGNA_GRP].size();
        }
        //groups that are assigned to fleets
        //iterate through divisions
        gssis.clear();
        for (auto div : sData[SUBS_VPN_DIV_LIST][it.first])
        {
            if (sData[SUBS_DIV_FLEET_LIST].count(div) == 0)
                continue;
            //iterate through fleets
            for (auto fleet : sData[SUBS_DIV_FLEET_LIST][div])
            {
                if (sFleetGrps.count(fleet) == 0)
                    continue;
                //a static group can be assigned to more than one fleet;
                //put them in a set to avoid duplication before getting the
                //size
                gssis.insert(sFleetGrps[fleet].begin(),
                             sFleetGrps[fleet].end());
            }
        }
        size += gssis.size();
        if (size > 0)
            oss << "VPN-" << it.first << " GSSI-Static: " << size << '\n';
        if (dgnaInd > 0)
            oss << "VPN-" << it.first << " GSSI-DGNA-Ind: " << dgnaInd
                << '\n';
        if (dgnaGrp > 0)
            oss << "VPN-" << it.first << " GSSI-DGNA-Grp: " << dgnaGrp
                << '\n';
    } //for (auto &it : sData[SUBS_VPN_DIV_LIST])
    //content of each division
    for (const auto &it : sData[SUBS_DIV_FLEET_LIST])
    {
        size = it.second.size();
        if (size > 0)
            oss << "DIV-" << it.first << " Fleet: " << size << '\n';
    }
    //content of each fleet
    for (const auto &it : sData[SUBS_FLEET_ISSI_LIST])
    {
        size = it.second.size();
        if (size > 0)
            oss << "Fleet-" << it.first << " ISSI: " << size << '\n';
        if (sFleetGrps.count(it.first) == 0)
            continue;
        size = sFleetGrps[it.first].size();
        if (size > 0)
            oss << "Fleet-" << it.first << " Group: " << size << '\n';
    }
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        for (const auto &it : sData[SUBS_FLEET_MOB_LIST])
        {
            size = it.second.size();
            if (size > 0)
                oss << "Fleet-" << it.first << " MobISSI: " << size << '\n';
        }
    }
    //content of each GSSI
    for (const auto &it : sData[SUBS_GSSI_SSI_LIST])
    {
        size = it.second.size();
        if (size > 0)
            oss << "GSSI-" << it.first << " SSI: " << size << '\n';
    }
    //attached members of each GSSI
    for (const auto &it : sData[SUBS_GSSI_ATTACH_LIST])
    {
        size = it.second.size();
        if (size > 0)
            oss << "GSSI-" << it.first << " Attached-ISSI: " << size << '\n';
    }
#ifdef SERVERAPP
    //branch data
    for (const auto &it : sBranchMap)
    {
        oss << "Branch-" << it.first << ' '
            << Utils::fromHexString(it.second.name) << " Cluster-"
            << it.second.cluster << ", Disp: "
            << getBranchIdCount(it.first, BRID_DISP);
        for (const auto &it2 : it.second.idMap)
        {
            if (it2.second.empty())
                continue;
            switch (it2.first)
            {
                case BRID_MOB:
                    oss << ", Mobile: ";
                    break;
                case BRID_SSI:
                    oss << ", SSI: ";
                    break;
                default:
                    continue; //should not occur
            }
            oss << getBranchIdCount(it.first, it2.first);
        }
        oss << '\n';
    }
#endif //SERVERAPP
    return oss.str();
}

void SubsData::addClient(int id, const string &domain, bool disp, bool haveLock)
{
    auto &idMap((disp)? sDispatcherIds: sMobileIds);
    if (haveLock)
    {
        idMap[id] = domain;
    }
    else
    {
        Locker lock(&sDataLock);
        idMap[id] = domain;
    }
}

void SubsData::removeResource(int id, bool disp, bool haveLock)
{
    if (!haveLock)
        PalLock::take(&sDataLock);
    if (disp)
    {
        sDispatcherIds.erase(id);
    }
    else
    {
        sMobileIds.erase(id);
        //remove from grp attachments
        auto &dm(sData[SUBS_GSSI_ATTACH_LIST]);
        auto it = dm.begin();
        while (it != dm.end())
        {
            if (it->second.erase(id) != 0 && it->second.empty())
                it = dm.erase(it);
            else
                ++it;
        }
#ifndef SERVERAPP
        grpUncDetach(id, 0, true);
#endif
    }
    if (!haveLock)
        PalLock::release(&sDataLock);
}

string SubsData::getClientData(bool disp, bool inclHeader, char sep)
{
    const auto &idMap((disp)? sDispatcherIds: sMobileIds);
    ostringstream oss;
    Locker lock(&sDataLock);
    if (inclHeader)
        oss << ((disp)? "Dispatcher": "Mobile") << " IDs(" << idMap.size()
            << "): ";
    if (!idMap.empty())
    {
        auto it = idMap.begin();
        oss << it->first;
        for (++it; it!=idMap.end(); ++it)
        {
            oss << sep << it->first;
        }
    }
    return oss.str();
}

string SubsData::getClientDomain(int id, bool disp)
{
    const auto &idMap((disp)? sDispatcherIds: sMobileIds);
    if (idMap.count(id) == 0)
        return "";
    return idMap.at(id);
}

bool SubsData::isMobile(int id)
{
    Locker lock(&sDataLock);
    //check for online clients first
    if (sMobileIds.count(id) != 0)
        return true;
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        for (const auto &it : sData[SUBS_FLEET_MOB_LIST])
        {
            if (it.second.count(id) != 0)
                return true;
        }
    }
    return false;
}

string SubsData::getIssiName(int issi, int type)
{
    Locker lock(&sDataLock);
    if (isReady())
    {
        IssiType2DescMapT::iterator it;
        if (type >= 0)
        {
            it = sTypeIssiDesc.find(type);
            if (it != sTypeIssiDesc.end() && it->second.count(issi) == 0)
                it = sTypeIssiDesc.end();
        }
        else //find in all types
        {
            for (it=sTypeIssiDesc.begin(); it!=sTypeIssiDesc.end(); ++it)
            {
                if (it->second.count(issi) != 0)
                    break;
            }
        }
        if (it != sTypeIssiDesc.end())
#ifdef SERVERAPP
            return it->second[issi].name();
#else
            return it->second[issi];
#endif
    }
    return "";
}

#ifdef SERVERAPP
bool SubsData::validIssi(int issi, int cid, bool disp)
#else
bool SubsData::validIssi(int issi)
#endif
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
#ifdef SERVERAPP
    if (getIssiFleetHaveLock(issi) >= 0)
    {
        if (cid == 0 || sClusterCount.size() <= 1)
            return true; //single cluster or no branching
        int type = (disp)? BRID_DISP: BRID_MOB;
        int cidCluster = -1;
        int issiCluster = -1;
        for (const auto &it : sBranchMap)
        {
            if (cidCluster < 0 && it.second.hasId(type, cid))
            {
                cidCluster = it.second.cluster;
                if (cidCluster == issiCluster)
                    return true;
            }
            if (issiCluster < 0 &&
                (it.second.hasId(BRID_SSI, issi) ||
                 it.second.hasId(BRID_MOB, issi)))
            {
                issiCluster = it.second.cluster;
                if (cidCluster == issiCluster)
                    return true;
            }
        }
    }
    return false;
#else
    return (getIssiFleetHaveLock(issi) >= 0);
#endif
}

bool SubsData::validGssi(int gssi)
{
    Locker lock(&sDataLock);
    return validGssiHaveLock(gssi);
}

string SubsData::getGrpName(int gssi)
{
    Locker lock(&sDataLock);
    if (isReady() && sGssiDesc.count(gssi) != 0)
#ifdef SERVERAPP
        return sGssiDesc[gssi].name();
#else
        return sGssiDesc[gssi];
#endif
    return "";
}

int SubsData::getGrpType(int gssi)
{
    Locker lock(&sDataLock);
    for (const auto &itv : sVpnGrps) //vpn iterator
    {
        for (const auto &itg : itv.second) //group type iterator
        {
            if (itg.second.count(gssi) != 0)
                return itg.first;
        }
    }
    return MsgSp::Value::SUBS_TYPE_GRP;
}

bool SubsData::getGrpMembers(int gssi, IdSetT &ssis)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    if (sData[SUBS_GSSI_SSI_LIST].count(gssi) == 0)
        return false;
    ssis.insert(sData[SUBS_GSSI_SSI_LIST][gssi].begin(),
                sData[SUBS_GSSI_SSI_LIST][gssi].end());
    return !ssis.empty();
}

string SubsData::getGrpMembers(int gssi)
{
    IdSetT ssis;
    getGrpMembers(gssi, ssis);
    ostringstream oss;
    oss << "Group " << gssi << " - members(" << ssis.size() << "): "
        << Utils::toStringWithRange(ssis);
    return oss.str();
}

bool SubsData::getGrpAttachedMembers(Int2IdsMapT &data)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    data = sData[SUBS_GSSI_ATTACH_LIST];
#ifndef SERVERAPP
    for (const auto &it : sData[SUBS_GSSI_ATTACH_LIST_UNC])
    {
        data[it.first].insert(it.second.begin(), it.second.end());
    }
#endif
    return !data.empty();
}

bool SubsData::getGrpAttachedMembers(int gssi, IdSetT &issis, bool unc)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    if (sData[SUBS_GSSI_ATTACH_LIST].count(gssi) == 0)
        return false;
    issis.insert(sData[SUBS_GSSI_ATTACH_LIST][gssi].begin(),
                 sData[SUBS_GSSI_ATTACH_LIST][gssi].end());
#ifndef SERVERAPP
    if (unc && sData[SUBS_GSSI_ATTACH_LIST_UNC].count(gssi) != 0)
        issis.insert(sData[SUBS_GSSI_ATTACH_LIST_UNC][gssi].begin(),
                     sData[SUBS_GSSI_ATTACH_LIST_UNC][gssi].end());
#endif
    return !issis.empty();
}

string SubsData::getGrpAttachedMembers(int gssi, bool header, bool multiline)
{
    IdSetT issis;
    getGrpAttachedMembers(gssi, issis, false);
    ostringstream oss;
    if (header)
        oss << "Group " << gssi << " - attached members(" << issis.size()
            << "): " << Utils::toStringWithRange(issis);
    else if (issis.empty())
        return "";
    else if (multiline)
        oss << Utils::toStringWithRange(issis, "\n");
    else
        oss << '{' << Utils::toStringWithRange(issis, ", ") << '}';
    return oss.str();
}

bool SubsData::isGrpAttachedMember(int issi, int gssi)
{
    if (isReady())
    {
        Locker lock(&sDataLock);
        const auto &d(sData[SUBS_GSSI_ATTACH_LIST]);
        if (d.count(gssi) != 0 && d.at(gssi).count(issi) != 0)
            return true;
#ifndef SERVERAPP
        const auto &d2(sData[SUBS_GSSI_ATTACH_LIST_UNC]);
        if (d2.count(gssi) != 0 && d2.at(gssi).count(issi) != 0)
            return true;
#endif
    }
    return false;
}

bool SubsData::isGrpAttachedMember(int issi, const IdSetT &gssis)
{
    if (isReady())
    {
        Locker lock(&sDataLock);
        const auto &d(sData[SUBS_GSSI_ATTACH_LIST]);
#ifndef SERVERAPP
        const auto &d2(sData[SUBS_GSSI_ATTACH_LIST_UNC]);
#endif
        for (auto i : gssis)
        {
            if (d.count(i) != 0 && d.at(i).count(issi) != 0)
                return true;
#ifndef SERVERAPP
            if (d2.count(i) != 0 && d2.at(i).count(issi) != 0)
                return true;
#endif
        }
    }
    return false;
}

string SubsData::getIssis(bool mob, int fleet, char sep)
{
    if (!isReady())
        return "";
    if (fleet == FLEET_NONE)
        fleet = sFleet;
    int key = (mob)? SUBS_FLEET_MOB_LIST: SUBS_FLEET_ISSI_LIST;
    Locker lock(&sDataLock);
    if (fleet == FLEET_NONE)
    {
        //network-mode: return all ISSIs
        string issis;
        if (sData.count(key) != 0)
        {
            for (auto &it : sData[key])
            {
                if (!it.second.empty())
                {
                    if (!issis.empty())
                        issis += sep;
                    issis += Utils::toString(it.second, sep);
                }
            }
        }
        return issis;
    }
    if (sData.count(key) != 0 && sData[key].count(fleet) != 0 &&
        !sData[key][fleet].empty())
        return Utils::toString(sData[key][fleet], sep);
    return "";
}

bool SubsData::getIssis(IdSetT &issis, bool mob, int fleet)
{
    if (!isReady())
        return false;
    if (fleet == FLEET_NONE)
        fleet = sFleet;
    int key = (mob)? SUBS_FLEET_MOB_LIST: SUBS_FLEET_ISSI_LIST;
    Locker lock(&sDataLock);
    if (fleet == FLEET_NONE)
    {
        //network-mode: return all ISSIs
        if (sData.count(key) != 0)
        {
            for (auto &it : sData[key])
            {
                if (!it.second.empty())
                    issis.insert(it.second.begin(), it.second.end());
            }
        }
        return true;
    }
    if (sData.count(key) != 0 && sData[key].count(fleet) != 0)
    {
        issis = sData[key][fleet];
        return true;
    }
    return false;
}

int SubsData::getGssiVpn(int gssi)
{
    Locker lock(&sDataLock);
    return getGssiVpnHaveLock(gssi);
}

bool SubsData::setStateDownloading()
{
    if (sState == STATE_DOWNLOADING)
        return false;
    sState = STATE_DOWNLOADING;
    return true;
}

inline void SubsData::setTimestamp(MsgSp *msg)
{
#ifdef SERVERAPP
    //set current timestamp and add to msg
    sTimestamp = time(NULL);
    if (msg != 0)
        msg->addField(MsgSp::Field::TIMESTAMP, sTimestamp);
#else
    //save server timestamp from msg
    if (msg != 0)
        msg->getFieldVal<time_t>(MsgSp::Field::TIMESTAMP, sTimestamp);
#endif
}

void SubsData::processMonMsg(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in SubsData::processMonMsg" == 0);
        return;
    }
    int    container;
    int    content;
    string valStr;
    switch (msg->getType())
    {
        case MsgSp::Type::MON_GRP_ATTACH_DETACH:
        {
            content = msg->getFieldInt(MsgSp::Field::ISSI);
            if (content == MsgSp::Value::UNDEFINED)
                break;
            auto &dm(sData[SUBS_GSSI_ATTACH_LIST]);
            container = msg->getFieldInt(MsgSp::Field::GRP_ATTACH_DETACH_MODE);
            if (container == MsgSp::Value::UNDEFINED)
            {
                //msg has a list of groups attached to the ISSI
                IdSetT gssis;
                if (Utils::fromStringWithRange(
                                    msg->getFieldString(MsgSp::Field::GRP_LIST),
                                    gssis) > 0)
                {
                    for (auto i : gssis)
                    {
                        dm[i].insert(content);
                    }
#ifndef SERVERAPP
                    grpUncDetach(content, 0, true); //confirmed attached
#endif
                }
                break;
            }
#ifndef SERVERAPP
            //on client, add list of affected GSSIs into msg to enable group
            //active state updates
            IdSetT gssis;
#endif
            if (container == MsgSp::Value::GRP_ATT_DET_DETACH_ALL)
            {
                //remove ISSI from all groups
#ifdef SERVERAPP
                for (auto &it : dm)
                {
                    it.second.erase(content);
                }
#else
                for (auto &it : dm)
                {
                    if (it.second.erase(content) != 0)
                        gssis.insert(it.first);
                }
                grpUncDetach(content, 0, true); //all grps because detached all
#endif
            }
            MsgSp::NestedListT nv;
            if (msg->getFieldVals(MsgSp::Field::GRP_ID, nv))
            {
                //v is a vector<int> with 3 elements:
                //Attach-Detach-Type, COU (for Attach)/Detach reason, GSSI
                for (auto &v : nv)
                {
                    if (v.size() < 3)
                        continue;
                    //reuse 'container' to store affected GSSI
                    if (v[0] == MsgSp::Value::GRP_ATT_DET_TYPE_DETACH)
                    {
                        container = -v[2]; //negative for removal
                    }
                    else if (v[1] == MsgSp::Value::COU_SCANNED_SELECTED_GRP)
                    {
                        container = v[2];
                    }
                    else
                    {
                        //for other COUs, terminal does not automatically
                        //attach to this GSSI if it is already attached to
                        //another - check for current attachment
                        auto it = dm.begin();
                        for (; it!=dm.end(); ++it)
                        {
                            if (it->second.count(content) != 0)
                            {
                                container = 0; //attached
                                break;
                            }
                        }
                        //if not attached - consider attached to this now
                        if (it == dm.end())
                            container = v[2];
                    }
                    if (container != 0)
                    {
                        if (container < 0)
                        {
                            container = -container;
                            dm[container].erase(content);
#ifndef SERVERAPP
                            grpUncDetach(content, container, true); //this grp
#endif
                        }
                        else
                        {
                            dm[container].insert(content);
#ifndef SERVERAPP
                            grpUncDetach(content, 0, true); //all grps
#endif
                        }
#ifndef SERVERAPP
                        gssis.insert(container);
#endif
                    }
                } //for (auto &v : nv)
            } //if (msg->getFieldVals(MsgSp::Field::GRP_ID, nv))
#ifndef SERVERAPP
            if (!gssis.empty())
                msg->addField(MsgSp::Field::GRP_LIST,
                              Utils::toString(gssis,
                                              MsgSp::Value::LIST_DELIMITER));
#endif
            break;
        }

        case MsgSp::Type::MON_LOC:
        {
            if (msg->getFieldInt(MsgSp::Field::LOC_UPDATE_TYPE) ==
                MsgSp::Value::LOC_UPDATE_TYPE_DEREGISTER)
            {
                //remove ISSI from all group attachment lists
                content = msg->getFieldInt(MsgSp::Field::ISSI);
                for (auto &it : sData[SUBS_GSSI_ATTACH_LIST])
                {
                    it.second.erase(content);
                }
#ifndef SERVERAPP
                grpUncDetach(content, 0, true);
#endif
            }
            break;
        }

        case MsgSp::Type::MON_SSDGNA_ASSIGN:
        {
            content = msg->getFieldInt(MsgSp::Field::ASG_DEASSIGN_STATUS);
            if (content == MsgSp::Value::ASGD_ASSIGNED ||
                content == MsgSp::Value::ASGD_PENDING_ASSIGN)
            {
                //add the assigned ISSI/GSSI to the DGNA group
                sData[SUBS_GSSI_SSI_LIST]
                     [msg->getFieldInt(MsgSp::Field::GSSI)].
                       insert(msg->getFieldInt(MsgSp::Field::AFFECTED_USER));
            }
            break;
        }

        case MsgSp::Type::MON_SSDGNA_DEASSIGN:
        {
            if (msg->getFieldInt(MsgSp::Field::ASG_DEASSIGN_STATUS) ==
                MsgSp::Value::ASGD_DEASSIGNED)
            {
                container = msg->getFieldInt(MsgSp::Field::GSSI);
                content = msg->getFieldInt(MsgSp::Field::AFFECTED_USER);
                //remove the deassigned group member ISSI/GSSI
                if (sData[SUBS_GSSI_SSI_LIST].count(container) != 0)
                    sData[SUBS_GSSI_SSI_LIST][container].erase(content);
                //ISSI may have been attached to the group but not yet
                //detached - remove that too
                if (sData[SUBS_GSSI_ATTACH_LIST].count(container) != 0)
                    sData[SUBS_GSSI_ATTACH_LIST][container].erase(content);
#ifndef SERVERAPP
                grpUncDetach(content, container, true); //this grp only
#endif
            }
            break;
        }

        case MsgSp::Type::MON_SSDGNA_DEFINE:
        {
            container = msg->getFieldInt(MsgSp::Field::GSSI);
            switch (msg->getFieldInt(MsgSp::Field::DGNA_GRP_TYPE))
            {
                case MsgSp::Value::DGNA_GT_GRP:
                    content = MsgSp::Value::SUBS_TYPE_DGNA_GRP;
                    break;
                case MsgSp::Value::DGNA_GT_INDIVIDUAL:
                default:
                    content = MsgSp::Value::SUBS_TYPE_DGNA_IND;
                    break;
            }
            //add group to the VPN
            sVpnGrps[msg->getFieldInt(MsgSp::Field::VPN)][content]
                .insert(container);
            //add group description
#ifdef SERVERAPP
            if (!setGssiDesc(container,
                             msg->getFieldString(MsgSp::Field::GRP_NAME)))
            {
                //name retained from existing - update in msg
                msg->addField(MsgSp::Field::GRP_NAME, getGssiDesc(container));
            }
            //add field for client
            msg->addField(MsgSp::Field::SUBS_CONTENT_TYPE, content);
#else
            setGssiDesc(container, msg->getFieldString(MsgSp::Field::GRP_NAME));
#endif
            break;
        }

        case MsgSp::Type::MON_SSDGNA_DELETE:
        {
            container = msg->getFieldInt(MsgSp::Field::GSSI);
            sData[SUBS_GSSI_SSI_LIST].erase(container);
            content = MsgSp::Value::SUBS_TYPE_DGNA_GRP;
            //loop to erase from VPN and determine the DGNA group type
            for (auto &vit : sVpnGrps)
            {
                if (vit.second[MsgSp::Value::SUBS_TYPE_DGNA_IND]
                       .erase(container) != 0)
                {
                    content = MsgSp::Value::SUBS_TYPE_DGNA_IND;
                    //a DGNA-Ind could be a member of a DGNA-Grp in the same
                    //VPN - erase it from all DGNA-Grps
                    for (auto i : vit.second[MsgSp::Value::SUBS_TYPE_DGNA_GRP])
                    {
                        if (sData[SUBS_GSSI_SSI_LIST].count(i) != 0)
                            sData[SUBS_GSSI_SSI_LIST][i].erase(container);
                    }
                    break;
                }
                if (vit.second[MsgSp::Value::SUBS_TYPE_DGNA_GRP]
                       .erase(container) != 0)
                    break;
            }
            msg->addField(MsgSp::Field::SUBS_CONTENT_TYPE, content);
            eraseGssiDesc(container);
            break;
        }

        case MsgSp::Type::MON_START:
        case MsgSp::Type::MON_STOP:
        {
            //only from STM and mobile clients - mobile grp attach/detach
            auto &dm(sData[SUBS_GSSI_ATTACH_LIST]);
            IdSetT gssis;
            if (Utils::fromStringWithRange(
                        msg->getFieldString(MsgSp::Field::SSI_LIST), gssis) > 0)
            {
                content = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
                if (msg->getType() == MsgSp::Type::MON_START)
                {
                    for (auto i : gssis)
                    {
                        dm[i].insert(content);
                    }
                }
                else
                {
                    for (auto i : gssis)
                    {
                        if (dm[i].erase(content) != 0 && dm[i].empty())
                            dm.erase(i);
                    }
                }
            }
            break;
        }

        case MsgSp::Type::MON_SUBS_ASSIGN:
        {
            container = msg->getFieldInt(MsgSp::Field::GSSI);
            content = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
            if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                MsgSp::Value::SUBS_TYPE_FLEET)
            {
                sData[SUBS_GSSI_FLEET_LIST][container].insert(content);
                sFleetGrps[content].insert(container);
            }
            else //user id type is ISSI
            {
                sData[SUBS_GSSI_SSI_LIST][container].insert(content);
                Int2IdsMapT &issis(sData[SUBS_FLEET_ISSI_LIST]);
                //check the fleet of this ISSI to add this Static-Group to
                //that fleet
                for (const auto &it : issis)
                {
                    if (issis[it.first].count(content) != 0)
                        sFleetGrps[it.first].insert(container);
                }
            }
            break;
        }

        case MsgSp::Type::MON_SUBS_DEASSIGN:
        {
            container = msg->getFieldInt(MsgSp::Field::GSSI);
            if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                MsgSp::Value::SUBS_TYPE_FLEET)
            {
                if (sData[SUBS_GSSI_FLEET_LIST].count(container) != 0)
                    sData[SUBS_GSSI_FLEET_LIST][container].erase(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
            }
            else if (sData[SUBS_GSSI_SSI_LIST].count(container) != 0)
            {
                //user id type ISSI
                sData[SUBS_GSSI_SSI_LIST][container].erase(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
            }
            //do not remove this Static-Group from the fleet
            break;
        }

        case MsgSp::Type::MON_SUBS_DEFINE:
        {
            container = msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_ID);
            switch (msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_TYPE))
            {
                case MsgSp::Value::SUBS_TYPE_DIVISION:
                    if (sFleet == FLEET_NONE)
                        sData[SUBS_DIV_FLEET_LIST][container].insert(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
                    break;
                case MsgSp::Value::SUBS_TYPE_FLEET:
                    if (sFleet == FLEET_NONE || sFleet == container)
                    {
                        switch (msg->getFieldInt(
                                               MsgSp::Field::SUBS_CONTENT_TYPE))
                        {
#ifdef SERVERAPP
                            //NEBULA uses ROOT for mobile
                            case MsgSp::Value::SUBS_TYPE_ROOT:
                                //change field for client
                                msg->addField(MsgSp::Field::SUBS_CONTENT_TYPE,
                                              MsgSp::Value::SUBS_TYPE_MOBILE);
                                //fallthrough
#endif
                            case MsgSp::Value::SUBS_TYPE_MOBILE:
                                content = SUBS_FLEET_MOB_LIST; //mobile addition
                                break;
                            default:
                                content = SUBS_FLEET_ISSI_LIST;
                                break;
                        }
                        sData[content][container].insert(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
                    }
                    break;
                case MsgSp::Value::SUBS_TYPE_VPN:
                    if (sFleet == FLEET_NONE)
                        sData[SUBS_VPN_DIV_LIST][container].insert(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
                    break;
                case MsgSp::Value::UNDEFINED:
                    //client login or mobile added
                    content = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
                    if (content == MsgSp::Value::SUBS_TYPE_DISPATCHER)
                    {
                        addClient(msg->getFieldInt(
                                                 MsgSp::Field::SUBS_CONTENT_ID),
                                  msg->getFieldString(MsgSp::Field::VOIP_GW),
                                  true, true);
                    }
                    else if (content == MsgSp::Value::SUBS_TYPE_MOBILE)
                    {
                        content = msg->getFieldInt(
                                                 MsgSp::Field::SUBS_CONTENT_ID);
                        addClient(content,
                                  msg->getFieldString(MsgSp::Field::VOIP_GW),
                                  false, true);
#ifndef SERVERAPP
                        setIssiDesc(content, TERMINALTYPE_PHONE, "", false);
#else
                        if (CfgManager::stmNwk())
#endif
                        {
                            //also add to fleet list in case not yet there
                            container = (sFleet != FLEET_NONE)? sFleet:
                                          msg->getFieldInt(MsgSp::Field::FLEET);
                            if (container > 0)
                                sData[SUBS_FLEET_MOB_LIST][container]
                                    .insert(content);
                        }
                    }
                    break;
                default:
                    break; //should never occur - do nothing
            }
            break;
        }

        case MsgSp::Type::MON_SUBS_DELETE:
        {
            int type;
            container = msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_ID);
            content = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
            switch (msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_TYPE))
            {
                case MsgSp::Value::SUBS_TYPE_DIVISION:
                    if (sData[SUBS_DIV_FLEET_LIST].count(container) != 0)
                    {
                        sData[SUBS_DIV_FLEET_LIST][container].erase(content);
                        sData[SUBS_FLEET_ISSI_LIST].erase(content);
                        if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
                            sData[SUBS_FLEET_MOB_LIST].erase(content);
                    }
                    break;
                case MsgSp::Value::SUBS_TYPE_FLEET:
                    switch (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE))
                    {
#ifdef SERVERAPP
                        //NEBULA uses ROOT for mobile
                        case MsgSp::Value::SUBS_TYPE_ROOT:
                            //change field for client
                            msg->addField(MsgSp::Field::SUBS_CONTENT_TYPE,
                                          MsgSp::Value::SUBS_TYPE_MOBILE);
                            //fallthrough
#endif
                        case MsgSp::Value::SUBS_TYPE_MOBILE:
                            type = SUBS_FLEET_MOB_LIST; //mobile deletion
                            break;
                        default:
                            type = SUBS_FLEET_ISSI_LIST;
                            //rmv from grp assignments, but do not rmv empty grp
                            for (auto &it : sData[SUBS_GSSI_SSI_LIST])
                            {
                                it.second.erase(content);
                            }
                            break;
                    }
                    if (sData[type].count(container) != 0)
                    {
                        sData[type][container].erase(content);
                        if (sData[type][container].empty())
                            sData[type].erase(container);
                    }
                    else
                    {
                        //remove from whichever fleet found
                        for (auto &it : sData[type])
                        {
                            if (it.second.erase(content) != 0)
                            {
                                if (it.second.empty())
                                    sData[type].erase(it.first);
                                break;
                            }
                        }
                    }
                    setIssiDesc(content, "");
                    removeResource(content, false, true);
                    break;
                case MsgSp::Value::SUBS_TYPE_VPN:
                    type = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
                    if (type == MsgSp::Value::SUBS_TYPE_DIVISION)
                    {
                        if (sData[SUBS_VPN_DIV_LIST].count(container) != 0)
                        {
                            sData[SUBS_VPN_DIV_LIST][container]
                                .erase(content);
                            sData[SUBS_DIV_FLEET_LIST].erase(content);
                        }
                    }
                    else //SUBS_TYPE_GRP
                    {
                        if (sFleet != FLEET_NONE &&
                            type == MsgSp::Value::SUBS_TYPE_GRP)
                            sFleetGrps[sFleet].erase(content);
                        else if (sVpnGrps.count(container) != 0 &&
                                 sVpnGrps[container].count(type) != 0)
                            sVpnGrps[container][type].erase(content);
                        eraseGssiDesc(content);
                    }
                    break;
                case MsgSp::Value::UNDEFINED:
                    //client logout or mobile deleted
                    type = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
                    if (type == MsgSp::Value::SUBS_TYPE_DISPATCHER)
                    {
                        removeResource(content, true, true);
                    }
                    else
                    {
#ifndef SERVERAPP
                        //get all grp attachments including unconfirmed - for
                        //simplicity, do this here instead of inside
                        //removeResource()
                        IdSetT gssis;
                        for (const auto &it : sData[SUBS_GSSI_ATTACH_LIST])
                        {
                            if (it.second.count(content) != 0)
                                gssis.insert(it.first);
                        }
                        for (const auto &it : sData[SUBS_GSSI_ATTACH_LIST_UNC])
                        {
                            if (it.second.count(content) != 0)
                                gssis.insert(it.first);
                        }
                        if (!gssis.empty())
                            msg->addField(MsgSp::Field::GRP_LIST,
                                          Utils::toString(gssis,
                                                 MsgSp::Value::LIST_DELIMITER));
#endif
                        removeResource(content, false, true);
#ifdef SERVERAPP
                        if (CfgManager::stmNwk())
#endif
                        {
                            container = msg->getFieldInt(MsgSp::Field::FLEET);
                            if (container > 0)
                            {
                                //mobile deleted - use sFleet if defined
                                if (sFleet != FLEET_NONE)
                                    container = sFleet;
                                if (sData[SUBS_FLEET_MOB_LIST].count(container)
                                    != 0)
                                    sData[SUBS_FLEET_MOB_LIST][container]
                                        .erase(content);
                            }
                        }
                    }
                    break;
                default:
                    break; //should never occur - do nothing
            }
            break;
        }

        case MsgSp::Type::MON_SUBS_DESC:
        {
            switch (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE))
            {
                case MsgSp::Value::SUBS_TYPE_GRP:
                case MsgSp::Value::SUBS_TYPE_DGNA_IND:
                case MsgSp::Value::SUBS_TYPE_DGNA_GRP:
#ifdef SERVERAPP
                    //if group has local name, do not send this to client
                    if (!setGssiDesc(
                                msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID),
                                msg->getFieldString(MsgSp::Field::DESC)))
                        msg->removeField(MsgSp::Field::DESC);
#else
                    setGssiDesc(msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID),
                                msg->getFieldString(MsgSp::Field::DESC));
#endif
                    break;
#ifdef SERVERAPP
                case MsgSp::Value::SUBS_TYPE_ROOT: //NEBULA uses ROOT for mobile
                    //change field for client
                    msg->addField(MsgSp::Field::SUBS_CONTENT_TYPE,
                                  MsgSp::Value::SUBS_TYPE_MOBILE);
                    //fallthrough
#endif
                case MsgSp::Value::SUBS_TYPE_ISSI:
#ifdef SERVERAPP
                    container = setIssiDesc(
                                msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID),
                                msg->getFieldString(MsgSp::Field::DESC));
                    //if subs has local name, do not send this to client
                    if (container >= 0)
                        msg->addField(MsgSp::Field::TERMINAL_TYPE, container);
                    else
                        msg->removeField(MsgSp::Field::DESC);
#else
                case MsgSp::Value::SUBS_TYPE_MOBILE:
                    setIssiDesc(msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID),
                                msg->getFieldInt(MsgSp::Field::TERMINAL_TYPE),
                                msg->getFieldString(MsgSp::Field::DESC));
#endif
                    break;
                default:
                    //reject type not yet supported
                    msg->removeField(MsgSp::Field::DESC);
                    break;
            }
            break;
        }

        case MsgSp::Type::MON_SUBS_PERMISSION:
        {
            container = msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_ID);
            if (msg->getFieldInt(MsgSp::Field::SUBS_CONTAINER_TYPE) ==
                MsgSp::Value::SUBS_TYPE_DIVISION)
            {
                //container type is Division and content is a fleet
                if (msg->getFieldInt(MsgSp::Field::SUBS_PERMISSION) ==
                    MsgSp::Value::SUBS_PERMISSION_GRANTED)
                    sData[SUBS_DIV_FLEET_LIST][container].insert(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
                else
                    sData[SUBS_DIV_FLEET_LIST][container].erase(
                               msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID));
            }
            else if (msg->getFieldInt(MsgSp::Field::SUBS_PERMISSION) ==
                     MsgSp::Value::SUBS_PERMISSION_GRANTED)
            {
                //container type is VPN and content is a group
                content = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
                int type = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
                //in fleet-mode, a static group always belongs to the fleet
                if (sFleet != FLEET_NONE &&
                    type == MsgSp::Value::SUBS_TYPE_GRP)
                    sFleetGrps[sFleet].insert(content);
                else
                    sVpnGrps[container][type].insert(content);
#ifdef SERVERAPP
                if (!setGssiDesc(content,
                                 msg->getFieldString(MsgSp::Field::DESC)))
                {
                    //name retained from existing - update in msg
                    msg->addField(MsgSp::Field::DESC, getGssiDesc(content));
                }
#else
                setGssiDesc(content, msg->getFieldString(MsgSp::Field::DESC));
#endif
            }
            else
            {
                content = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID);
                int type = msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE);
                if (sFleet != FLEET_NONE &&
                    type == MsgSp::Value::SUBS_TYPE_GRP)
                    sFleetGrps[sFleet].erase(content);
                else if (sVpnGrps.count(container) != 0 &&
                         sVpnGrps[container].count(type) != 0)
                    sVpnGrps[container][type].erase(content);
                eraseGssiDesc(content);
            }
            break;
        }

        default:
        {
            break; //should never occur - do nothing
        }
    } //switch (msg->getType())
}

int SubsData::getTotalSize(int type)
{
    int size = 0;
    for (const auto &it : sData[type])
    {
        size += it.second.size();
    }
    return size;
}

int SubsData::getGssiVpnHaveLock(int gssi)
{
    for (const auto &git : sVpnGrps)
    {
        for (const auto &it : git.second)
        {
            if (it.second.count(gssi) != 0)
            {
                //gssi is either a DGNA group or an empty static group
                return git.first;
            }
        }
    }
    //iterate through fleets to find the group
    for (const auto &git : sFleetGrps)
    {
        if (git.second.count(gssi) == 0)
            continue;
        //iterate through divisions to find the fleet
        for (const auto &dit : sData[SUBS_DIV_FLEET_LIST])
        {
            if (dit.second.count(git.first) == 0)
                continue;
            //iterate through vpns to find the division
            for (const auto &it : sData[SUBS_VPN_DIV_LIST])
            {
                if (it.second.count(dit.first) != 0)
                    return it.first;
            }
        }
    }
    return 0;
}

int SubsData::toGrpType(int type)
{
    if (type == MsgSp::Value::SUBS_TYPE_DGNA_GRP)
        return MsgSp::Value::GRP_TYPE_DGNA_GRP;
    if (type == MsgSp::Value::SUBS_TYPE_DGNA_IND)
        return MsgSp::Value::GRP_TYPE_DGNA_IND;
    return MsgSp::Value::GRP_TYPE_STATIC;
}

int SubsData::fromGrpType(int type)
{
    if (type == MsgSp::Value::GRP_TYPE_DGNA_GRP)
        return MsgSp::Value::SUBS_TYPE_DGNA_GRP;
    if (type == MsgSp::Value::GRP_TYPE_DGNA_IND)
        return MsgSp::Value::SUBS_TYPE_DGNA_IND;
    return MsgSp::Value::SUBS_TYPE_GRP;
}

bool SubsData::setGssiDesc(int gssi, const string &name)
{
#ifdef SERVERAPP
    sGssiDesc[gssi].nwkName = name;
    if (!sGssiDesc[gssi].locName.empty())
        return false; //has local name, so this new name is not used now
#else
    if (name.empty())
        sGssiDesc.erase(gssi);
    else
        sGssiDesc[gssi] = name;
#endif
    return true;
}

void SubsData::eraseGssiDesc(int gssi)
{
#ifdef SERVERAPP
    //skip if gssi has local name
    if (sGssiDesc[gssi].locName.empty())
#endif
        sGssiDesc.erase(gssi);
}

bool SubsData::validGssiHaveLock(int gssi)
{
    for (const auto &it : sFleetGrps)
    {
        if (it.second.count(gssi) != 0)
            return true;
    }
    return (getGssiVpnHaveLock(gssi) != 0);
}

int SubsData::getIssiFleetHaveLock(int issi)
{
    if (sData.count(SUBS_FLEET_ISSI_LIST) != 0)
    {
        for (const auto &it : sData[SUBS_FLEET_ISSI_LIST])
        {
            if (it.second.count(issi) != 0)
                return it.first;
        }
    }
    if (sData.count(SUBS_FLEET_MOB_LIST) != 0)
    {
        for (const auto &it : sData[SUBS_FLEET_MOB_LIST])
        {
            if (it.second.count(issi) != 0)
                return it.first;
        }
    }
    return -1;
}

#ifdef SERVERAPP
inline string SubsData::getGssiDesc(int gssi)
{
    if (sGssiDesc.count(gssi) != 0)
        return sGssiDesc[gssi].name();
    return "";
}

int SubsData::setIssiDesc(int issi, const string &name, int type)
{
    for (auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
        {
            it.second[issi].nwkName = name;
            if (it.second[issi].locName.empty())
                return it.first;
            return -1;
        }
    }
    //not found - use default type
    sTypeIssiDesc[type][issi] = SsiData(false, name);
    return type;
}

inline string SubsData::getIssiDesc(int issi)
{
    for (auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
            return it.second[issi].name();
    }
    return "";
}

void SubsData::removeBranchRange(int min, int max, Int2IntMapT &data)
{
    auto it = data.find(min);
    if (it == data.end())
        it = data.begin();
    while (it != data.end())
    {
        if (it->first > max)
            break; //beyond range - done
        if (it->second < min)
        {
            ++it; //below range - go next
            continue;
        }
        //overlap
        if (it->second >= max)
        {
            //split/shrink/erase existing depending on it->first
            if (it->second > max)
                data[max + 1] = it->second; //split or shrink
            //note: after element insertion above, 'it' remains valid because
            //the new element is higher, but must not be used for continued
            //iteration
            max = 0; //done after truncating/erasing below
        }
        if (it->first < min)
        {
            it->second = min - 1; //truncate
            ++it;
        }
        else
        {
            it = data.erase(it);
        }
        if (max == 0)
            break;
    }
}

void SubsData::branchRangeToIds(const Int2IntMapT &data, IdSetT &ids)
{
    int i;
    for (auto &it : data)
    {
        for (i=it.first; i<=it.second; ++i)
        {
            ids.insert(i);
        }
    }
}

int SubsData::getBranchIdCount(int branch, int type)
{
    int n = 0;
    if (sBranchMap.count(branch) != 0)
    {
        switch (type)
        {
            case BRID_DISP:
                n = sBranchMap[branch].cids.size();
                break;
            case BRID_SVR:
                n = sBranchMap[branch].svrUsrs.size();
                break;
            default:
                if (sBranchMap[branch].idMap.count(type) != 0)
                {
                    for (const auto &it : sBranchMap[branch].idMap[type])
                    {
                        n += (it.second - it.first + 1);
                    }
                }
                break;
        }
    }
    return n;
}

string SubsData::getBranchIdsHaveLock(int branch, int type)
{
    if (sBranchMap.count(branch) != 0)
    {
        switch (type)
        {
            case BRID_DISP:
            {
                return Utils::toStringWithRange(sBranchMap[branch].cids, " ");
            }
            case BRID_SVR:
            {
                if (!sBranchMap[branch].svrUsrs.empty())
                    return Utils::toString(sBranchMap[branch].svrUsrs, ' ');
                break;
            }
            default:
            {
                if (sBranchMap[branch].idMap.count(type) != 0)
                {
                    ostringstream oss;
                    for (const auto &it : sBranchMap[branch].idMap[type])
                    {
                        oss << it.first;
                        if (it.second != it.first)
                            oss << '-' << it.second;
                        oss << ' ';
                    }
                    return oss.str();
                }
                break;
            }
        }
    }
    return "";
}

bool SubsData::isInBranchHaveLock(const IdSetT &branches, int type, int id)
{
    for (auto b : branches)
    {
        if (sBranchMap.count(b) != 0 && sBranchMap[b].hasId(type, id))
            return true;
    }
    //not in given branches
    if (sClusterCount.size() > 1)
        return false;
    //single-cluster - check other branches
    for (const auto &it : sBranchMap)
    {
        if (branches.count(it.first) == 0 && it.second.hasId(type, id))
            return false; //in another branch
    }
    return true; //unassigned - valid in all branches
}

bool SubsData::isInBranchHaveLock(int type, int id, int branch)
{
    if (branch != 0)
        return (sBranchMap.count(branch) != 0 &&
                sBranchMap[branch].hasId(type, id));
    for (const auto &it : sBranchMap)
    {
        if (it.second.hasId(type, id))
            return true;
    }
    return false;
}

#else
void SubsData::setIssiDesc(int           issi,
                           int           terminalType,
                           const string &name,
                           bool          overwrite)
{
    //store even if name is empty, for the type
    for (auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
        {
            if (!overwrite)
                return;
            if (it.first == terminalType)
            {
                it.second[issi] = name; //only update name
                return;
            }
            it.second.erase(issi);      //type changed
            break;
        }
    }
    sTypeIssiDesc[terminalType][issi] = name;
}

void SubsData::setIssiDesc(int issi, const string &name)
{
    for (auto &it : sTypeIssiDesc)
    {
        if (it.second.count(issi) != 0)
        {
            if (name.empty())
                it.second.erase(issi);
            else
                it.second[issi] = name;
            break;
        }
    }
}

void SubsData::getVpnGroups(const Int2IdsMapT &vpnGrps,
                            int                type,
                            Ssi2DescMapT      &groups)
{
    auto mit = vpnGrps.find(type);
    if (mit != vpnGrps.end()) //vpnGrps has the group type
        getGrpNames(mit->second, groups);
}

void SubsData::getGrpNames(const IdSetT &gssis, Ssi2DescMapT &groups)
{
    for (auto i : gssis)
    {
        if (sGssiDesc.count(i) != 0)
            groups[i] = sGssiDesc[i];
        else
            groups[i] = "";
    }
}
#endif //SERVERAPP

bool SubsData::parseBranchRanges(const string &str, Int2IntMapT &data)
{
    if (str.empty())
        return false;
    int min;
    int max;
    char c;
    string token;
    istringstream is(str);
    while (getline(is, token, ' '))
    {
        if (token.empty())
            continue;
        istringstream ist(token);
        if (!(ist >> min) || min <= 0)
            continue; //invalid
        //either one value or range min-max
        if (!(ist >> c >> max) || c != '-' || max < min)
            max = min;
        //entries are not sorted and may overlap
        addBranchRange(min, max, data);
    }
    return !data.empty();
}

void SubsData::addBranchRange(int min, int max, Int2IntMapT &data)
{
    auto it = data.find(min);
    if (it == data.end())
        it = data.begin();
    while (it != data.end())
    {
        if (it->first > max + 1)
            break; //beyond range - done
        if (it->second < min - 1)
        {
            ++it; //below range - go next
        }
        //overlap or adjacent
        else if (it->first <= min)
        {
            if (it->second >= max)
            {
                min = 0; //covered by existing - done
                break;
            }
            min = it->first; //will extend existing - go next to check overlap
            ++it;
        }
        //current starts within (min,max+1]
        else if (it->second > max)
        {
            max = it->second; //extend range, remove existing, and done
            data.erase(it);
            break;
        }
        else
        {
            it = data.erase(it); //current swallowed - delete and go next
        }
    }
    if (min != 0)
        data[min] = max;
}
