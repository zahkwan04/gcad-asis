/**
 * The Settings implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Settings.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Mazdiana Makmor
 */
#include "Socket.h"
#include "Settings.h"

using namespace std;

static const string DELIM(" = ");

Settings *Settings::sInstance(0);

#ifdef _WIN32
PalLock::LockT Settings::sSingletonLock; //no init needed
#elif defined QT_CORE_LIB
PalLock::LockT Settings::sSingletonLock(QMutex::Recursive);
#else
PalLock::LockT Settings::sSingletonLock = PTHREAD_MUTEX_INITIALIZER;
#endif

Settings &Settings::instance()
{
    static bool isCreated = false;

    //a modified Double Checked Locking Pattern for thread-safe Singleton,
    //lock obtained only before/during creation
    if (!isCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!isCreated) //thread-safety double check
        {
            sInstance = new Settings();
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                isCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

void Settings::destroy()
{
    delete sInstance;
    sInstance = 0;
}

ostream &operator<<(ostream &os, const Settings &cfg)
{
    os << "#Settings::\n" << Props::toString(cfg.mProperties, DELIM, "\n");
    return os;
}

Settings &Settings::set(int key, const string &value)
{
    Props::set(mProperties, key, value);
    return *this;
}

void Settings::remove(int key)
{
    mProperties.erase(key);
}

bool Settings::validate(int &invKey)
{
    mValid = (checkVal(Props::FLD_CFG_SERVERIP, invKey) &&
              checkVal(Props::FLD_CFG_SERVERPORT, invKey) &&
              checkVal(Props::FLD_CFG_MAP_TERM_STALE1, invKey, false) &&
              checkVal(Props::FLD_CFG_MAP_TERM_STALELAST, invKey, false));
    return mValid;
}

bool Settings::validate(int key, const string &value, bool doSet)
{
    int val;
    switch (key)
    {
        case Props::FLD_CFG_INC_ICONDIR:
        case Props::FLD_CFG_INCFILTER_ADDRSTATE:
        case Props::FLD_CFG_INCFILTER_CATEGORY:
        case Props::FLD_CFG_INCFILTER_PRIORITY:
        case Props::FLD_CFG_INCFILTER_STATE:
        case Props::FLD_CFG_LOGFILE:
        case Props::FLD_CFG_MMS_DOWNLOADDIR:
        case Props::FLD_CFG_PTT_CHAR:
        case Props::FLD_CFG_SDSTEMPLATE:
        {
            //these are fields that can have empty value
            break; //do nothing
        }
        case Props::FLD_CFG_MAP_TERM_STALE1:
        {
            val = Utils::fromString<int>(value);
            if (val > 0 && val < TERMINAL_TIMER_MIN_MINUTES)
                return false;
            break;
        }
        case Props::FLD_CFG_MAP_TERM_STALELAST:
        {
            val = Utils::fromString<int>(value);
            if (val > 0 &&
                (val <
                 TERMINAL_TIMER_MIN_MINUTES +
                     get<int>(Props::FLD_CFG_MAP_TERM_STALE1)))
                return false;
            break;
        }
        case Props::FLD_CFG_SERVERIP:
        {
            vector<string> ips;
            if (!Utils::fromString(value, ips, ' ') ||
                !Socket::validateIp(ips[0]) ||
                (ips.size() > 1 && !Socket::validateIp(ips[1])))
                return false;
            break;
        }
        case Props::FLD_CFG_SERVERPORT:
        {
            vector<int> ports;
            if (!Utils::fromString(value, ports, ' ') || ports[0] <= 0 ||
                (ports.size() > 1 && ports[1] <= 0))
                return false;
            break;
        }
        default:
        {
            if (value.empty())
                return false;
            break;
        }
    }
    if (doSet)
        Props::set(mProperties, key, value);
    return true;
}

inline bool Settings::checkVal(int key, int &invKey, bool isMandatory)
{
    bool retVal;
    if (mProperties.count(key) != 0)
        retVal = validate(key, Props::get(mProperties, key), false);
    else
        retVal = !isMandatory;
    if (!retVal)
        invKey = key;
    return retVal;
}
