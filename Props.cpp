/**
 * General purpose properties implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Props.cpp 1899 2025-01-22 05:42:07Z rosnin $
 * @author Nur Zawanah Ishak
 */
#include <ostream>

#include "Locker.h"
#include "Props.h"

using namespace std;

static const string VAL_EMPTY("");

const string Props::VAL_INCIDENT_DEFTYPE ("Others");
const string Props::VAL_ROUTE_START      ("Start");
const string Props::VAL_ROUTE_END        ("End");
const string Props::VAL_TERMINAL_INVALID ("LocInvalid");
const string Props::VAL_TERMINAL_VALID   ("LocValid");
const string Props::VAL_TERMINAL_STALE   ("LocStale");

//static initializers
Props::ValueMapT Props::sFieldNames(createFieldNames());
Props::ValsT     Props::sVals;
#ifdef _WIN32
PalLock::LockT   Props::sValsLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT   Props::sValsLock(QMutex::Recursive);
#else
PalLock::LockT   Props::sValsLock = PTHREAD_MUTEX_INITIALIZER;
#endif

const string &Props::getFieldName(int fld)
{
    if (sFieldNames.count(fld) == 0)
        fld = FLD_UNDEFINED;
    return sFieldNames[fld];
}

int Props::getFieldId(const string &name)
{
    ValueMapT::const_iterator it = sFieldNames.begin();
    for (; it!=sFieldNames.end(); ++it)
    {
        if (name == it->second)
            return it->first;
    }
    return FLD_UNDEFINED;
}

string Props::toString(const ValueMapT &vals,
                       const string    &pairDelim,
                       const string    &delim)
{
    if (vals.empty())
        return VAL_EMPTY;
    ostringstream os;
    ValueMapT::const_iterator it = vals.begin();
    os << getFieldName(it->first) << pairDelim << it->second;
    for (++it; it!=vals.end(); ++it)
    {
        os << delim << getFieldName(it->first) << pairDelim << it->second;
    }
    return os.str();
}

void Props::set(ValueMapT &vals, int fld, const string &value)
{
    vals[fld] = value;
}

void Props::set(ValueMapT    &vals,
                int           fld,
                const string &value1,
                const string &value2)
{
    vals[fld].assign(value1).append(1, VAL_COMBINER).append(value2);
}

void Props::set(Props::ValueMapT &vals, int fld, int src1, int src2)
{
    vals[fld].assign(get(vals, src1)).append(1, VAL_COMBINER)
             .append(get(vals, src2));
}

const string &Props::get(const ValueMapT &vals, int fld)
{
    if (vals.count(fld) != 0)
        return vals.at(fld);
    return VAL_EMPTY;
}

const string &Props::get(const ValueMapT &vals, const string &fldName)
{
    int fld = FLD_UNDEFINED;
    ValueMapT::const_iterator it = sFieldNames.begin();
    for (; it!=sFieldNames.end(); ++it)
    {
        if (fldName == it->second)
        {
            fld = it->first;
            break;
        }
    }
    return get(vals, fld);
}

string Props::getPart(const ValueMapT &vals, int fld, int idx)
{
    vector<string> parts;
    if (Utils::fromString(get(vals, fld), parts, VAL_COMBINER, true) &&
        idx < parts.size())
        return parts[idx];
    return VAL_EMPTY;
}

const string *Props::getStr(const string &str)
{
    Locker lock(&sValsLock);
    std::pair<ValsT::iterator, bool> p = sVals.insert(str);
    return &*(p.first);
}

void Props::remove(ValueMapT &vals, int fld)
{
    vals.erase(fld);
}

void Props::add(const ValueMapT &src, ValueMapT &dst)
{
    ValueMapT::const_iterator it = src.begin();
    for (; it!=src.end(); ++it)
    {
        dst[it->first] = it->second;
    }
}

Props::ValueMapT Props::createFieldNames()
{
    ValueMapT v;
    v[FLD_UNDEFINED]               = "UNDEFINED";

    v[FLD_CFG_AUDIO_IN]            = "AudioIn";
    v[FLD_CFG_AUDIO_OUT]           = "AudioOut";
    v[FLD_CFG_BRANCH]              = "Branch";
    v[FLD_CFG_BRANCH_ALLOWED]      = "BranchAllowed";
    v[FLD_CFG_CAMERA]              = "Camera";
    v[FLD_CFG_CAMERA_RES]          = "CameraResolution";
    v[FLD_CFG_COLORTHEME]          = "ColorTheme";
    v[FLD_CFG_CONTACTS]            = "Contacts";
    v[FLD_CFG_GPS_MON]             = "GpsMon";
    v[FLD_CFG_GRPCALL_AUTOJOIN]    = "GrpCallAutoJoin";
    v[FLD_CFG_HELPDESKNUM]         = "HelpDeskNum";
    v[FLD_CFG_INC_ICONDIR]         = "IncIconDir";
    v[FLD_CFG_INCFILTER_ADDRSTATE] = "IncFilterAddrState";
    v[FLD_CFG_INCFILTER_CATEGORY]  = "IncFilterCategory";
    v[FLD_CFG_INCFILTER_PRIORITY]  = "IncFilterPriority";
    v[FLD_CFG_INCFILTER_STATE]     = "IncFilterState";
    v[FLD_CFG_LOGFILE]             = "LogFile";
    v[FLD_CFG_LOGLEVEL]            = "LogLevel";
    v[FLD_CFG_MAP_CTR_RSC_CALL]    = "MapCtrRscInCall";
    v[FLD_CFG_MAP_MAXSCALE]        = "MapMaxScale";
    v[FLD_CFG_MAP_SEA]             = "MapSea";
    v[FLD_CFG_MAP_TERM_LBL]        = "MapTermLbl";
    v[FLD_CFG_MAP_TERM_STALE1]     = "MapTermStale1";
    v[FLD_CFG_MAP_TERM_STALELAST]  = "MapTermStaleLast";
    v[FLD_CFG_MMS_DOWNLOADDIR]     = "MMSDownloadDir";
    v[FLD_CFG_MONITOR_RETAIN]      = "MonRetain";
    v[FLD_CFG_MSG_TMR_INTERVAL]    = "MsgTimerInterval";
    v[FLD_CFG_PTT_ALT]             = "PttAlt";
    v[FLD_CFG_PTT_CHAR]            = "PttChar";
    v[FLD_CFG_PTT_CTRL]            = "PttCtrl";
    v[FLD_CFG_RSC_DSP_GRP]         = "RscDspGrp";
    v[FLD_CFG_RSC_DSP_SUBS]        = "RscDspSubs";
    v[FLD_CFG_SDSTEMPLATE]         = "SDSTemplate";
    v[FLD_CFG_SERVERIP]            = "ServerIP";
    v[FLD_CFG_SERVERPORT]          = "ServerPort";

    v[FLD_COORDINATES]             = "Coordinates";
    v[FLD_COORDINATES_MULTILINE]   = "CoordsMultiLine";
    v[FLD_DISTANCE]                = "Distance";
    v[FLD_FULLNAME]                = "FullName";
    v[FLD_ICON]                    = "Icon";
    v[FLD_ID]                      = "shape_id";
    v[FLD_IS_PUBLIC]               = "Public";
    v[FLD_ISSI]                    = "ISSI";
    v[FLD_KEY]                     = "Key";
    v[FLD_KEY_NEW]                 = "NewKey";
    v[FLD_LAT]                     = "Lat";
    v[FLD_LAYERNAME]               = "LayerName";
    v[FLD_LBL]                     = "Lbl";
    v[FLD_LOC_TIME_VALID]          = "LocTimeValid";
    v[FLD_LOC_TIMESTAMP]           = "LocTimeStamp";
    v[FLD_LOC_TIMESTAMP_VALID]     = "LocTimeStampValid";
    v[FLD_LON]                     = "Lon";
    v[FLD_OWNER]                   = "Owner";
    v[FLD_STATUS]                  = "Status";
    v[FLD_TRAILING]                = "Trailing";
    v[FLD_TYPE]                    = "Type";
    v[FLD_USERPOI_ADDR]            = "Addr";
    v[FLD_USERPOI_CATEGORY]        = "Cat";
    v[FLD_USERPOI_DESC]            = "Desc";
    v[FLD_USERPOI_NAME]            = "Name";
    v[FLD_USERPOI_SHORTNAME]       = "ShortName";
    return v;
}
