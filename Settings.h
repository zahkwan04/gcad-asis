/**
 * The Settings module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2021. All Rights Reserved.
 *
 * @file
 * @version $Id: Settings.h 1483 2021-05-25 11:20:31Z zulzaidi $
 * @author Mazdiana Makmor
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "PalLock.h"
#include "Props.h"
#include "Utils.h"

class Settings
{
public:
    //minimum timer interval for terminal location status check on map
    static const int TERMINAL_TIMER_MIN_MINUTES = 5;

    /**
     * Instantiates the singleton if it has not been created.
     *
     * @return The instance.
     */
    static Settings &instance();

    /**
     * Deletes the single instance.
     */
    static void destroy();

    friend std::ostream &operator<<(std::ostream &os, const Settings &cfg);

    bool isValid() const { return mValid; }

    /**
     * Inserts or modifies a configuration value corresponding to a key.
     *
     * @param[in] key   The key.
     * @param[in] value The value.
     * @return This object.
     */
    Settings &set(int key, const std::string &value);

    /**
     * Inserts or modifies a configuration value corresponding to a key.
     *
     * @param[in] key   The key.
     * @param[in] value The value.
     * @tparam    T     The value type.
     * @return This object.
     */
    template<class T>
    Settings &set(int key, T value);

    /**
     * Gets the value corresponding to a key. If not found, returns the
     * specified default value.
     *
     * @param[in] key      The key.
     * @param[in] defValue The default value.
     * @tparam    T        The value type.
     * @return The value. The default value if the key does not exist.
     */
    template<class T>
    T get(int key, T defValue) const;

    /**
     * Gets the value corresponding to a key.
     *
     * @param[in] key The key.
     * @tparam    T   The value type.
     * @return The value. 0 or empty string if the key does not exist.
     */
    template<class T>
    T get(int key) const;

    /**
     * Gets a list of values corresponding to a key.
     * This is for a configuration entry containing multiple delimited values
     * (strings or numbers).
     *
     * @param[in]  key   The key.
     * @param[out] v     The vector to be populated. The caller is
     *                   responsible for clearing it first if required.
     * @param[in]  delim The value delimiter.
     * @tparam     T     The value type.
     * @return The number of values in the list.
     */
    template<class T>
    size_t getList(int key, std::vector<T> &v, char delim = ' ') const;

    /**
     * Gets a list of values corresponding to a key.
     * This is for a configuration entry containing multiple delimited values
     * (strings or numbers).
     *
     * @param[in]  key   The key.
     * @param[out] s     The set to be populated. The caller is responsible
     *                   for clearing it first if required.
     * @param[in]  delim The value delimiter.
     * @tparam     T     The value type.
     * @return The number of values in the list.
     */
    template<class T>
    size_t getList(int key, std::set<T> &s, char delim = ' ') const;

    /**
     * Template specialization for a list of integers that may be stored as
     * ranges.
     */
    template<>
    size_t getList(int key, std::set<int> &s, char delim) const;

    /**
     * Removes a configuration entry (but not from the permanent storage).
     *
     * @param[in] key The key.
     */
    void remove(int key);

    /**
     * Validates all configuration entries.
     *
     * @param[out] invKey Invalid key in case of failure.
     * @return true if all entries are valid.
     */
    bool validate(int &invKey);

    /**
     * Validates a given configuration value and optionally sets it if valid.
     *
     * @param[in] key   The key.
     * @param[in] value The value.
     * @param[in] doSet true to set the value if valid.
     * @return true if valid.
     */
    bool validate(int key, const std::string &value, bool doSet);

private:
    bool                   mValid;
    Props::ValueMapT       mProperties;
    static Settings       *sInstance;
    static PalLock::LockT  sSingletonLock;

    /**
     * Constructor is private to prevent direct instantiation.
     */
    Settings() : mValid(false) {}

    /**
     * Prevent copy construction.
     */
    Settings(const Settings &);

    /**
     * Prevent assignment operation.
     */
    Settings &operator=(const Settings &);

    /**
     * Checks whether a configuration entry exists and is valid.
     *
     * @param[in]  key         The configuration key.
     * @param[out] invKey      The same configuration key if invalid.
     * @param[in]  isMandatory true if the key must exist.
     * @return true if entry is valid or a non-mandatory entry does not exist.
     */
    bool checkVal(int key, int &invKey, bool isMandatory = true);
};

template<class T>
Settings &Settings::set(int key, T value)
{
    Props::set(mProperties, key, value);
    return *this;
}

template<class T>
T Settings::get(int key, T defValue) const
{
    return Props::get<T>(mProperties, key, defValue);
}

template<class T>
T Settings::get(int key) const
{
    return Props::get<T>(mProperties, key);
}

template<class T>
size_t Settings::getList(int key, std::vector<T> &v, char delim) const
{
    Utils::fromString<T>(Props::get(mProperties, key), v, delim);
    return v.size();
}

template<class T>
size_t Settings::getList(int key, std::set<T> &s, char delim) const
{
    Utils::fromString<T>(Props::get(mProperties, key), s, delim);
    return s.size();
}

template<>
size_t Settings::getList(int key, std::set<int> &s, char delim) const
{
    Utils::fromStringWithRange(Props::get(mProperties, key), s, delim);
    return s.size();
}
#endif //SETTINGS_H
