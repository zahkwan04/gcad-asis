/**
 * Collection of utility functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2012-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Utils.h 1775 2023-10-27 03:33:04Z rosnin $
 * @author Mohd Rozaimi
 * @author Rashid Yatim
 */
#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <iomanip>   //setfill(), setw()
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace Utils
{
    typedef std::chrono::high_resolution_clock::time_point TimepointT;

    /**
     * Performs case insensitive string comparison.
     *
     * @param[in] a First string.
     * @param[in] b Second string.
     * @return true if the strings are equal.
     */
    bool strIequal(const std::string &a, const std::string &b);

    /**
     * Converts a hexadecimal string into a byte array.
     * Assumes that the string contains only hexadecimal characters without
     * validation. If the string length is odd, adds a '0' prefix before
     * conversion.
     * E.g. "414263313233" becomes {0x41,0x42,0x63,0x31,0x32,0x33},
     *      "00010AFF"     becomes {0x00,0x01,0x0A,0xFF},
     *      "ABCDE"        becomes {0x0A,0xBC,0xDE}.
     *
     * @param[in]  hexStr The hexadecimal string.
     * @param[out] len    The number of bytes in the returned array.
     * @return The byte array, or 0 if str is empty. Caller must destroy it with
     *         'delete []'.
     */
    char *fromHexString(const std::string &hexStr, int &len);

    /**
     * Converts a hexadecimal string into the represented ASCII string by first
     * converting it into a byte array using the other fromHexString().
     * This is meant for a hexadecimal string that is known to represent
     * printable characters. No validation is done.
     * E.g. "414263313233" becomes "ABc123".
     *
     * @param[in] hexStr The hexadecimal string.
     * @return The converted string.
     */
    std::string fromHexString(const std::string &hexStr);

    /**
     * Converts a hexadecimal string of Unicode values into wide string.
     * Each value must use either UTF8 or UCS2/UTF16 encoding, represented by
     * 2 or 4 hexadecimal characters in host byte order (big-endian).
     * E.g. either "41" or "0041" for "A".
     * This assumes the input string contains only hexadecimal characters
     * without validating.
     * If the string length is not even, the conversion is done with '0' prefix
     * padding.
     * E.g. "004100420063003100320033" and "414263313233" = "ABc123",
     *      "480065006C006C006F" and "48656C6C6F" = "Hello",
     * This is only to provide a 'best effort' support to print a Unicode
     * string. Non-ASCII characters, particularly beyond 2 bytes, may not be
     * shown correctly.
     *
     * @param[in] hexStr The hexadecimal string.
     * @return The converted string.
     */
    std::wstring hex2Wstring(const std::string &hexStr);

    /**
     * Converts a hexadecimal string of Unicode values into u16string.
     * Each value must use UCS2/UTF16 encoding, represented by 4 hexadecimal
     * characters in host byte order (big-endian).
     * This assumes the input string contains only hexadecimal characters
     * without validating.
     *
     * @param[in] hexStr The hexadecimal string.
     * @return The converted string.
     */
    std::u16string hex2u16string(const std::string &hexStr);

    /**
     * Converts a hexadecimal string of UTF16 Unicode values into hexadecimal
     * string of UTF8 values. Uses hex2u16string() to decode the UTF16 string
     * before doing conversion.
     *
     * @param[in] hexStr The UTF16 hexadecimal string.
     * @return The converted string.
     */
    std::string hexUtf16ToUtf8(const std::string &hexStr);

    /**
     * Converts a UCS2 Unicode string into UTF8.
     *
     * @param[in] ustr The UCS2 string.
     * @return The converted string.
     */
    std::string ucs2ToUtf8(std::u16string &ustr);

    /**
     * Converts a string of delimited strings into a std::vector.
     * Appends new entries to the given vector.
     *
     * @param[in]  str         The string.
     * @param[out] data        The converted data.
     * @param[in]  delimiter   The value delimiter in the string.
     * @param[in]  acceptEmpty true to accept empty string as an element.
     * @return true if successful.
     */
    bool fromString(const std::string        &str,
                    std::vector<std::string> &data,
                    char                      delimiter,
                    bool                      acceptEmpty = false);

    /**
     * Converts a string of delimited strings into a std::set.
     *
     * @param[in]  str       The string.
     * @param[out] data      The converted data.
     * @param[in]  delimiter The value delimiter in the string.
     * @return true if successful.
     */
    bool fromString(const std::string     &str,
                    std::set<std::string> &data,
                    char                   delimiter);

    /**
     * Converts a string of delimited integers into a set of unique integers.
     * The integers can appear as a single number or a range min-max with no
     * space around the '-', where max must be greater than min.
     * E.g. "1 99 2-8 30-55".
     * If a word starts with a number but does not follow the above format, the
     * starting number is taken and the rest is discarded.
     * E.g. "45abc", "45-", "45-a47", "45-30" are all converted to 45.
     * Other non-numeric words are discarded.
     *
     * @param[in]  str       The string.
     * @param[out] data      The converted data.
     * @param[in]  delimiter The integer delimiter.
     * @return The number of integers added to the data.
     */
    int fromStringWithRange(const std::string &str,
                            std::set<int>     &data,
                            char               delimiter = ',');

    /**
     * Converts an integer set into string which may contain ranges in the form
     * "...min1-max1<delimiter>min2-max2...".
     * A range is used for 3 or more consecutive values.
     *
     * @param[in] data       The set.
     * @param[in] delimiter  The value delimiter in the string.
     * @param[in] header     The output header, if any. Appended in the output
     *                       with the string "(<set size>): ".
     * @param[in] splitLines true to split output into lines with up to 8 items
     *                       each (fewer if there are range items).
     * @return The converted string.
     */
    std::string toStringWithRange(const std::set<int> &data,
                                  const std::string   &delimiter = ",",
                                  const std::string   &header = "",
                                  bool                 splitLines = false);

    /**
     * Converts an integer vector into string by putting the values into a set
     * and calling toStringWithRange(std::set<int>).
     * This means that duplicated values are discarded, and is mainly for
     * vectors that are known to have only unique values.
     *
     * @param[in] data       The vector.
     * @param[in] delimiter  The value delimiter in the string.
     * @param[in] header     The output header, if any. Appended in the output
     *                       with the string "(<set size>): ".
     * @param[in] splitLines true to split output into lines with up to 8 items
     *                       each (fewer if there are range items).
     * @return The converted string.
     */
    std::string toStringWithRange(const std::vector<int> &data,
                                  const std::string      &delimiter = ",",
                                  const std::string      &header = "",
                                  bool                    splitLines = false);

    /**
     * Converts a string of keys and a string of values into a std::map.
     *
     * @param[in]  keys        The keys.
     * @param[in]  values      The values.
     * @param[out] data        The converted data.
     * @param[in]  delimiter   The value delimiter in the string.
     * @param[in]  acceptEmpty true to accept empty values.
     * @return true if successful.
     */
    bool fromString(const std::string          &keys,
                    const std::string          &values,
                    std::map<int, std::string> &data,
                    char                        delimiter,
                    bool                        acceptEmpty = false);

    /**
     * Converts a floating number into a string.
     *
     * @param[in] val       The value.
     * @param[in] precision The floating point precision.
     * @return The string.
     */
    std::string toString(double val, int precision = 0);

    /**
     * Converts a std::map into a string of Key=Value pairs.
     *
     * @param[in] data      The map.
     * @param[in] delimiter The delimiter between pairs in the output.
     * @return The converted string.
     */
    std::string toString(
                 const std::map<std::string, std::string> &data,
                 const std::string                        &delimiter = ", ");

    /**
     * Converts a wstring into string.
     * This is a convenience function for simple conversion of a wstring that
     * contains ASCII characters only, for display or logging.
     * If the source contains characters beyond ASCII, the result is undefined.
     *
     * @param[in] wstr The source wstring.
     * @return The converted string.
     */
    std::string toString(const std::wstring &wstr);

    /**
     * Converts a timestamp into epoch time.
     *
     * @param[in] timestamp The timestamp. Order must be as in default format
     *                      "YYYY-MM-dd hh:mm:ss", but the separators may be
     *                      specified differently in the next parameter.
     * @param[in] fmt       The format specifier as per scanf() specifications
     *                      if using different separators from the default.
     * @return The time in seconds, or 0 for invalid input.
     */
    time_t getTimeVal(const std::string &timestamp,
                      const std::string &fmt = "%d-%d-%d %d:%d:%d");

    /**
     * Converts a timestamp into epoch time.
     *
     * @param[in] timestamp See getTimeVal().
     * @param[in] fmt       See getTimeVal().
     * @return The time in seconds, or empty string for invalid input.
     */
    std::string getTimeStr(const std::string &timestamp,
                           const std::string &fmt = "%d-%d-%d %d:%d:%d");

    /**
     * Formats a time structure value as "YYYY-MM-dd hh:mm:ss".
     *
     * @param[in] stm The time structure.
     * @return The formatted time.
     */
    std::string formatTime(const struct tm &stm);

    /**
     * Formats a time structure value as "Day dd/MM/yyyy HH:mm:ss".
     *
     * @param[in] stm The time structure.
     * @return The formatted time.
     */
    std::string formatTimeWithDayName(const struct tm &stm);

    /**
     * Formats a time value using either the other formatTime() or
     * formatTimeWithDayName().
     *
     * @param[in] t           The time value.
     * @param[in] withDayName true to use the second version.
     * @return The formatted time.
     */
    std::string formatTime(time_t t, bool withDayName = false);

    /**
     * Gets a timestamp.
     *
     * @param[in] formatted Determines the output format.
     * @return Timestamp string in the format
     *         "YYYY-MM-dd hh:mm:ss.millisecond" if formatted is true,
     *         "YYYYMMdd_hhmmss.millisecond" otherwise.
     */
    std::string getTimestamp(bool formatted = true);

    /**
     * Gets a timestamp formatted for a message: "dd/MM/YYYY hh:mm:ss".
     *
     * @return Timestamp string.
     */
    std::string getMsgTimestamp();

    /**
     * Gets a high resolution clock timepoint for current time, or epoch 0 time.
     *
     * @param[in] reset true to get 0 time.
     * @return The timepoint.
     */
    TimepointT getTimepoint(bool reset = false);

    /**
     * Checks whether a timepoint is valid, i.e. has non-zero value.
     *
     * @param[in] t The timepoint.
     * @return true if valid.
     */
    bool timepointValid(const TimepointT &t);

    /**
     * Gets the elapsed time in milliseconds from a start timepoint.
     * Returns 0 if the start is not set, i.e. has value 0.
     *
     * @param[in] start The start timepoint.
     * @return The elapsed time.
     */
    long timepointElapsedMs(const TimepointT &start);

    /**
     * Gets the transfer statistics (duration and speed) from a start timepoint.
     * Output format: "<min>m <sec>s <msec>ms, <speed>Mbps / <speed>kbps".
     * E.g. "678.271ms, 53.5333Mbps / 54818.1kbps"
     * Shows min and sec only if non-zero.
     * Shows speed only if the transfer size is given.
     * Shows Mbps only if speed is higher than 1024kbps.
     *
     * @param[in] start The start.
     * @param[in] sz    The transfer size.
     * @return The statistics.
     */
    std::string getTransferStats(TimepointT start, int64_t sz = 0);

    /**
     * Gets a random alphanumeric string with a random length within the
     * specified range.
     *
     * @param[in] minLen The minimum length. If <= 0 and maxLen is > 0, it is
     *                   set to 1.
     * @param[in] maxLen The maximum length. If < minLen, set to minLen.
     * @return The alphanumeric string, or empty string if both minLen and
     *         maxLen are <= 0.
     */
    std::string randomString(int minLen, int maxLen);

    /**
     * Scrambles a string with a key.
     *
     * @param[in] init      Initializer.
     * @param[in] data      The string to scramble.
     * @param[in] key       The key.
     * @param[in] appendKey true to append the key to the result.
     * @return The scrambled string.
     */
    std::string scramble(int                init,
                         const std::string &data,
                         const std::string &key,
                         bool               appendKey = true);

    /**
     * Converts data bytes into a hexadecimal string.
     * Each byte is converted to 2 uppercase hex characters, with the given
     * separator, if any, between bytes.
     * E.g. if sep == ",": "ABc123"       becomes "41,42,63,31,32,33",
     *                     {0,1,0xA,0xFF} becomes "00,01,0A,FF".
     *
     * @param[in] data The byte array.
     * @param[in] len  The number of bytes to convert.
     * @param[in] sep  The separator - omit if none required.
     * @return The hexadecimal string.
     */
    std::string toHexString(const char        *data,
                            int                len,
                            const std::string &sep = "");

    /**
     * Converts the beginning part of a string into a hexadecimal string.
     * @see toHexString(const char *data, ...).
     *
     * @param[in] str The string.
     * @param[in] len The number of characters to convert.
     * @param[in] sep The separator - omit if none required.
     * @return The hexadecimal string.
     */
    std::string toHexString(const std::string &str,
                            int                len,
                            const std::string &sep = "");

    /**
     * Converts a string into a hexadecimal string.
     * @see toHexString(const char *data, ...).
     *
     * @param[in] str The string.
     * @param[in] sep The separator - omit if none required.
     * @return The hexadecimal string.
     */
    std::string toHexString(const std::string &str,
                            const std::string &sep = "");

    /**
     * Converts a value into a hexadecimal string.
     * Pads the string with leading zeroes if length is specified but the value
     * is not large enough.
     *
     * @param[in] val The value.
     * @param[in] len The output string length. Ignored if too small for the
     *                value. Omit to use the minimum length required for the
     *                value.
     * @return The hexadecimal string.
     */
    std::string toHexString(unsigned int val, unsigned int len = 0);

    /**
     * Trims leading and trailing whitespaces from the given string.
     *
     * @param[in,out] str The string.
     * @return Length of the trimmed string in bytes.
     */
    size_t trim(std::string &str);

    /**
     * Removes single quotes from a string, only if the quote character is
     * present at both ends.
     *
     * @param[in] str The string.
     * @return The final string length.
     */
    size_t removeQuotes(std::string &str);

    /**
     * Checks whether a file path is unique, i.e. does not exist.
     * If so, do nothing.
     * Otherwise modifies it until unique.
     * 1. Splits the path into prefix and extension, which is the part starting
     *    with the last '.', if any.
     * 2. If extra prefix given, appends "-<extraPrefix>" to the prefix and
     *    checks "<prefix><ext>".
     * 3. If still not unique, appends "-1" to the prefix and checks
     *    "<prefix><ext>".
     * 4. If still not unique, keeps incrementing the number until unique.
     *
     * @param[in,out] path        The path.
     * @param[in]     extraPrefix The extra prefix.
     */
    void makeUniqueFilepath(std::string       &path,
                            const std::string &extraPrefix = "");

    /**
     * Gets a reference number that is unique for this application instance
     * across time. Main use is for a message reference.
     *
     * @return The reference number.
     */
    time_t getUniqueRef();

    /**
     * Converts a whole hexadecimal string, or just the first few characters,
     * into a value.
     * The length to be converted must not exceed twice the size of T in bytes,
     * since each byte is represented by 2 hexadecimal characters.
     *
     * @param[in] hexStr The hexadecimal string.
     * @param[in] len    The number of characters to convert. Omit to use the
     *                   entire string. Capped at twice the size of T.
     * @tparam    T      The value type. Must support the '>>' operator.
     * @return The value. 0 for invalid input.
     */
    template<class T>
    T fromHexString(const std::string &hexStr, unsigned int len = 0)
    {
        T v(0);
        if (len == 0)
            len = hexStr.size();
        if (len > (2 * sizeof(T)))
            len = 2 * sizeof(T);
        if (len < hexStr.size())
        {
            std::istringstream is(hexStr.substr(0, len));
            is >> std::hex >> v;
        }
        else
        {
            std::istringstream is(hexStr);
            is >> std::hex >> v;
        }
        return v;
    }

    /**
     * Converts the first word of a string into a value.
     *
     * @param[in] str The string.
     * @tparam    T   The value type. Must support the '>>' operator.
     * @return The value, or its 0 representation if conversion failed or
     *         string is empty.
     */
    template<class T>
    T fromString(const std::string &str)
    {
        T v(0);
        std::istringstream is(str);
        is >> v;
        return v;
    }

    template<>
    inline std::string fromString<std::string>(const std::string &str)
    {
        return str;
    }

    /**
     * Converts a string into a value by finding the first word that can be
     * successfully converted into the specified type.
     *
     * @param[in]  str  The string.
     * @param[out] data The converted data.
     * @tparam     T    The value type. Must support the '>>' operator.
     * @return true if successful.
     */
    template<class T>
    bool fromString(const std::string &str, T &data)
    {
        std::string        token;
        std::istringstream is(str);
        while (is >> token)
        {
            std::istringstream iss(token);
            if (iss >> data)
                return true;
        }
        return false;
    }

    /**
     * Converts a string of delimited values into a std::vector.
     *
     * @param[in]  str       The string.
     * @param[out] data      The converted data.
     * @param[in]  delimiter The value delimiter in the string.
     * @tparam     T         The value type. Must support the '>>' operator.
     * @return true if successful.
     */
    template<class T>
    bool fromString(const std::string &str,
                    std::vector<T>    &data,
                    char               delimiter)
    {
        size_t             oldSize = data.size();
        std::string        token;
        T                  val;
        std::istringstream is(str);
        while (getline(is, token, delimiter))
        {
            if (!token.empty())
            {
                std::istringstream iss(token);
                if (iss >> val)
                    data.push_back(val);
            }
        }
        return (data.size() > oldSize);
    }

    //specialization to support string tokens with embedded spaces
    template<class T>
    bool fromString(const std::string        &str,
                    std::vector<std::string> &data,
                    char                      delimiter)
    {
        size_t             oldSize = data.size();
        std::string        token;
        std::istringstream is(str);
        while (getline(is, token, delimiter))
        {
            if (!token.empty())
                data.push_back(token);
        }
        return (data.size() > oldSize);
    }

    /**
     * Converts a string of delimited values into a std::set.
     *
     * @param[in]  str       The string.
     * @param[out] data      The converted data.
     * @param[in]  delimiter The value delimiter in the string.
     * @tparam     T         The value type. Must support the '>>' operator.
     * @return true if successful.
     */
    template<class T>
    bool fromString(const std::string &str,
                    std::set<T>       &data,
                    char               delimiter)
    {
        size_t             oldSize = data.size();
        std::string        token;
        T                  val;
        std::istringstream is(str);
        while (getline(is, token, delimiter))
        {
            if (!token.empty())
            {
                std::istringstream iss(token);
                if (iss >> val)
                    data.insert(val);
            }
        }
        return (data.size() > oldSize);
    }

    //specialization
    template<>
    inline bool fromString(const std::string     &str,
                           std::set<std::string> &data,
                           char                   delimiter)
    {
        size_t             oldSize = data.size();
        std::string        token;
        std::istringstream is(str);
        while (getline(is, token, delimiter))
        {
            if (!token.empty())
                data.insert(token);
        }
        return (data.size() > oldSize);
    }

    /**
     * Converts a string of delimited value pairs into a std::map.
     * Does not support RHS value with embedded space, e.g. "5=the value".
     *
     * @param[in]  str           The string.
     * @param[out] data          The converted data.
     * @param[in]  listDelimiter The value delimiter in the string.
     * @param[in]  pairDelimiter The value pair delimiter.
     * @tparam     T             The value type. Must support the '>>'
     *                           operator.
     * @return true if successful.
     */
    template<class T>
    bool fromString(const std::string &str,
                    std::map<int, T>  &data,
                    char               listDelimiter,
                    char               pairDelimiter)
    {
        unsigned int       oldSize = data.size();
        int                val1;
        T                  val2;
        std::string        token;
        std::istringstream is(str);
        std::istringstream iss;
        if (pairDelimiter == ' ')
        {
            while (getline(is, token, listDelimiter))
            {
                iss.clear();
                iss.str(token);
                if (iss >> val1 >> val2)
                    data[val1] = val2;
            }
        }
        else
        {
            char c;
            while (getline(is, token, listDelimiter))
            {
                iss.clear();
                iss.str(token);
                if ((iss >> val1 >> c >> val2) && (c == pairDelimiter))
                    data[val1] = val2;
            }
        }
        return (data.size() > oldSize);
    }

    /**
     * Converts a value into string.
     *
     * @param[in] data The value.
     * @tparam    T    The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const T &data)
    {
        std::ostringstream os;
        os << data;
        return os.str();
    }

    template<>
    inline std::string toString<std::string>(const std::string &str)
    {
        return str;
    }

    /**
     * Converts a std::set into string with a single character delimiter.
     *
     * @param[in] data      The set.
     * @param[in] delimiter The value delimiter in the string.
     * @param[in] count     The number of items in the set to convert.
     *                      If 0 or greater than the set size, converts all.
     * @tparam    T         The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const std::set<T> &data,
                         char               delimiter,
                         int                count = 0)
    {
        if (data.empty())
            return "";
        std::ostringstream os;
        auto it = data.begin();
        os << *it;
        if (count > 0)
        {
            for (++it, --count; it!=data.end() && count>0; ++it, --count)
            {
                os << delimiter << *it;
            }
        }
        else
        {
            for (++it; it!=data.end(); ++it)
            {
                os << delimiter << *it;
            }
        }
        return os.str();
    }

    /**
     * Converts a std::set into string with a string delimiter.
     *
     * @param[in] data      The set.
     * @param[in] delimiter The value delimiter in the string.
     * @param[in] count     The number of items in the set to convert.
     *                      If 0 or greater than the set size, converts all.
     * @tparam    T         The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const std::set<T> &data,
                         const std::string &delimiter,
                         int                count = 0)
    {
        if (data.empty())
            return "";
        std::ostringstream os;
        auto it = data.begin();
        os << *it;
        if (count > 0)
        {
            for (++it, --count; it!=data.end() && count>0; ++it, --count)
            {
                os << delimiter << *it;
            }
        }
        else
        {
            for (++it; it!=data.end(); ++it)
            {
                os << delimiter << *it;
            }
        }
        return os.str();
    }

    /**
     * Converts a std::vector into string with a single character delimiter.
     *
     * @param[in] data      The vector.
     * @param[in] delimiter The value delimiter in the string.
     * @tparam    T         The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const std::vector<T> &data, char delimiter)
    {
        if (data.empty())
            return "";
        std::ostringstream os;
        auto it = data.begin();
        os << *it;
        for (++it; it!=data.end(); ++it)
        {
            os << delimiter << *it;
        }
        return os.str();
    }

    /**
     * Converts a std::vector into string with a string delimiter.
     *
     * @param[in] data      The vector.
     * @param[in] delimiter The value delimiter in the string.
     * @tparam    T         The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const std::vector<T> &data,
                         const std::string    &delimiter)
    {
        if (data.empty())
            return "";
        std::ostringstream os;
        auto it = data.begin();
        os << *it;
        for (++it; it!=data.end(); ++it)
        {
            os << delimiter << *it;
        }
        return os.str();
    }

    /**
     * Converts a std::map into string.
     *
     * @param[in] data          The map.
     * @param[in] listDelimiter The value delimiter between map entries.
     * @param[in] pairDelimiter The value pair delimiter.
     * @tparam    T             The value type. Must support the '<<' operator.
     * @return The converted string.
     */
    template<class T>
    std::string toString(const std::map<int, T> &data,
                         char                    listDelimiter,
                         char                    pairDelimiter)
    {
        if (data.empty())
            return "";
        std::ostringstream os;
        auto it = data.begin();
        os << it->first << pairDelimiter << it->second;
        for (++it; it!=data.end(); ++it)
        {
            os << listDelimiter << it->first << pairDelimiter << it->second;
        }
        return os.str();
    }

    /**
     * Converts a data array into hexadecimal string.
     * Each element is converted to the appropriate number of uppercase hex
     * characters, with the given separator, if any, between elements.
     *
     * @param[in] data The data array.
     * @param[in] len  The number of elements to convert.
     * @param[in] sep  The separator - omit if none required.
     * @return The hexadecimal string.
     */
    template<class T>
    std::string toHexString(const T *data, int len, const std::string &sep = "")
    {
        std::ostringstream os;
        os << std::setfill('0') << std::setw(2 * sizeof(T)) << std::hex
           << std::uppercase << *data;
        for (--len, ++data; len>0; --len, ++data)
        {
            os << sep << std::setw(2 * sizeof(T)) << *data; //setw is non-sticky
        }
        return os.str();
    }

    /**
     * Converts a value into hexadecimal string.
     * Pads the string with leading zeroes if length is specified but the value
     * is not large enough.
     *
     * @param[in] val The value.
     * @param[in] len The output length. Ignored if too small for the value.
     *                Omit to use the minimum length.
     * @return The hexadecimal string.
     */
    template<class T>
    std::string valToHexString(T val, int len = 0)
    {
        std::ostringstream os;
        if (len != 0)
            os << std::setfill('0') << std::setw(len) << std::hex
               << std::uppercase << val;
        else
            os << std::hex << std::uppercase << val;
        return os.str();
    }
}
#endif //UTILS_H
