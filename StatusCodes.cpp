/**
 * Status Codes manager implementation
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: StatusCodes.cpp 1752 2023-09-12 06:36:20Z hazim.rujhan $
 * @author Zahari Hadzir
 */
#include <iomanip>   //setw()
#include <vector>
#include <assert.h>

#include "Locker.h"
#include "StatusCodes.h"

using namespace std;

int                   StatusCodes::sState(StatusCodes::STATE_INVALID);
time_t                StatusCodes::sTimestamp(0);
StatusCodes::TypeMapT StatusCodes::sTypes;
StatusCodes::DataMapT StatusCodes::sData;
#ifdef _WIN32
PalLock::LockT        StatusCodes::sDataLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT        StatusCodes::sDataLock(QMutex::Recursive);
#else
PalLock::LockT        StatusCodes::sDataLock = PTHREAD_MUTEX_INITIALIZER;
#endif

void StatusCodes::init()
{
    sState = STATE_INVALID;
    sTimestamp = 0;
    sTypes.clear();
    sData.clear();
}

time_t StatusCodes::getTimestamp()
{
    Locker lock(&sDataLock);
    return sTimestamp;
}

string StatusCodes::getTimestampStr(bool showRaw)
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

//on client, or server with STM-nwk only
bool StatusCodes::processMsg(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in StatusCodes::processMsg" == 0);
        return false;
    }
    if (msg->getType() != MsgSp::Type::STATUS_DATA)
    {
        assert("StatusCodes::processMsg: Invalid message type" == 0);
        return false;
    }
    Locker lock(&sDataLock);
    if (!msg->hasField(MsgSp::Field::MSG_ACK))
    {
        //unsolicited update - STATUS_CODE_LIST and other related list fields
        //have exactly one item;
        //this is for either status type or status code
        int code = msg->getFieldInt(MsgSp::Field::STATUS_CODE_LIST);
        int type = msg->getFieldInt(MsgSp::Field::STATUS_TYPE_LIST);
        if (code != MsgSp::Value::UNDEFINED)
        {
            if (type == MsgSp::Value::UNDEFINED)
            {
                //code only - remove status code
                sData.erase(code);
            }
            else if (msg->hasField(MsgSp::Field::STATUS_TEXT_LIST))
            {
                //code, type and text - add/modify status code
                sData[code] = Data(type,
                           msg->getFieldString(MsgSp::Field::STATUS_TEXT_LIST));
            }
        }
        else if (type != MsgSp::Value::UNDEFINED)
        {
            if (msg->hasField(MsgSp::Field::DESC_LIST))
            {
                //type and desc - add/modify status type
                sTypes[type] = msg->getFieldString(MsgSp::Field::DESC_LIST);
            }
            else
            {
                //type only - remove status type
                sTypes.erase(type);
            }
        }
        setTimestamp(msg);
        //msg needs to be passed up to user layer
        return true;
    }
    if (!msg->isResultSuccessful())
    {
        sState = STATE_INVALID;
        return true;
    }
    //initial download - msg contains either actual lists or no data if already
    //up-to-date
    vector<int> vType;
    Utils::fromString(msg->getFieldString(MsgSp::Field::STATUS_TYPE_LIST),
                      vType, MsgSp::Value::LIST_DELIMITER);
    vector<int> vCode;
    Utils::fromString(msg->getFieldString(MsgSp::Field::STATUS_CODE_LIST),
                      vCode, MsgSp::Value::LIST_DELIMITER);
    if (vCode.empty() && vType.empty())
    {
        if (msg->hasField(MsgSp::Field::TIMESTAMP))
        {
            //data already up-to-date, nothing transferred
            sState = STATE_READY;
        }
        else
        {
            //first message in data download - prepare to receive new data
            sState = STATE_INVALID;
            sTypes.clear();
            sData.clear();
        }
        return false;
    }
    vector<string> vText;
    Utils::fromString(msg->getFieldString(MsgSp::Field::DESC_LIST), vText,
                      MsgSp::Value::LIST_DELIMITER, true);
    auto xIt = vText.begin();
    if (!vText.empty())
    {
        //getting list of status types
        for (auto i : vType)
        {
            if (xIt != vText.end())
                sTypes[i] = *xIt++;
            else
                sTypes[i] = ""; //put empty string if not enough descriptions
        }
        return false;
    }
    //getting list of status codes
    Utils::fromString(msg->getFieldString(MsgSp::Field::STATUS_TEXT_LIST),
                      vText, MsgSp::Value::LIST_DELIMITER);
    //ensure all vectors have the same size
    size_t minSize = vCode.size();
    if (minSize == vType.size())    //most likely case
    {
        ; //do nothing
    }
    else if (minSize < vType.size())
    {
        vType.resize(minSize);      //discard extra types
    }
    else
    {
        minSize = vType.size();
        vCode.resize(minSize);      //discard extra codes
    }
    if (minSize == vText.size())    //most likely case
    {
        ; //do nothing
    }
    else if (minSize < vText.size())
    {
        vText.resize(minSize);      //discard extra texts
    }
    else
    {
        minSize = vText.size();
        vCode.resize(minSize);      //discard extra codes
        vType.resize(minSize);      //discard extra types
    }
    auto tIt = vType.begin();
    xIt = vText.begin();
    for (auto i : vCode)
    {
        sData[i] = Data(*tIt++, *xIt++);
    }
    if (msg->getFieldVal<time_t>(MsgSp::Field::TIMESTAMP, sTimestamp))
        sState = STATE_READY;
    return false;
}

bool StatusCodes::getTypes(StatusCodes::TypeMapT &types)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    types = sTypes;
    return !types.empty();
}

string StatusCodes::getTypes()
{
    if (!isReady())
        return "";
    Locker lock(&sDataLock);
    ostringstream oss;
    oss << "Status Types (" << sTypes.size() << "):";
    for (const auto &it : sTypes)
    {
        oss << "\n  [" << it.first << "] " << it.second;
    }
    return oss.str();
}

void StatusCodes::setType(int type, const string &desc, bool doSetTimestamp)
{
    Locker lock(&sDataLock);
    sTypes[type] = Utils::toHexString(desc);
    if (doSetTimestamp)
        setTimestamp();
}

string StatusCodes::getTypeDesc(int type)
{
    Locker lock(&sDataLock);
    if (sTypes.count(type) == 0)
        return "";
    return sTypes[type];
}

void StatusCodes::removeType(int type)
{
    Locker lock(&sDataLock);
    sTypes.erase(type);
    setTimestamp();
}

bool StatusCodes::getData(DataMapT &data)
{
    if (!isReady())
        return false;
    Locker lock(&sDataLock);
    data = sData;
    return !data.empty();
}

string StatusCodes::getData()
{
    if (!isReady())
        return "";
    Locker lock(&sDataLock);
    ostringstream oss;
    oss << "Status Codes (" << sData.size() << "):";
    for (const auto &it : sData)
    {
        oss << "\n  [" << it.second.type << "] " << setw(5)
            << it.first << ' ' << it.second.text;
    }
    return oss.str();
}

void StatusCodes::setStatus(int           code,
                            int           type,
                            const string &text,
                            bool          doSetTimestamp)
{
    Locker lock(&sDataLock);
    sData[code] = Data(type, Utils::toHexString(text));
    if (doSetTimestamp)
        setTimestamp();
}

void StatusCodes::removeStatus(int code)
{
    Locker lock(&sDataLock);
    sData.erase(code);
    setTimestamp();
}

bool StatusCodes::getStatus(int code, int &type, string &text)
{
    Locker lock(&sDataLock);
    if (sData.count(code) != 0)
    {
        type = sData[code].type;
        text = sData[code].text;
        return true;
    }
    return false;
}

string StatusCodes::getStatusText(int code)
{
    Locker lock(&sDataLock);
    if (sData.count(code) == 0)
        return "";
    return sData[code].text;
}

void StatusCodes::setStateReady()
{
    sState = STATE_READY;
    Locker lock(&sDataLock);
    setTimestamp();
}

bool StatusCodes::setStateDownloading(bool doSet)
{
    if (doSet)
    {
        if (sState == STATE_DOWNLOADING)
            return false;
        sState = STATE_DOWNLOADING;
    }
    else if (sState == STATE_DOWNLOADING)
    {
        sState = STATE_INVALID; //disconnected while downloading
    }
    else
    {
        return false; //no change
    }
    return true;
}

inline void StatusCodes::setTimestamp(MsgSp *msg)
{
    //if on client, save server timestamp from msg
    if (msg == 0 ||
        !msg->getFieldVal<time_t>(MsgSp::Field::TIMESTAMP, sTimestamp))
    {
        //on server, set current timestamp and add to msg
        sTimestamp = time(NULL);
        if (msg != 0)
            msg->addField(MsgSp::Field::TIMESTAMP, sTimestamp);
    }
}
