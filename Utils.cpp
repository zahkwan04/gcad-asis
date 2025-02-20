/**
 * Utility functions implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2012-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Utils.cpp 1775 2023-10-27 03:33:04Z rosnin $
 * @author Mohd Rozaimi
 * @author Rashid Yatim
 */
#if defined(_WIN32) || defined(WIN32) || __GNUC__ >= 5
#include <codecvt>      //codecvt_utf8_utf16
#include <locale>       //wstring_convert
#endif
#include <algorithm>    //equal()
#include <cmath>        //modf()
#include <stdio.h>      //sscanf
#include <stdlib.h>     //rand
#include <sys/stat.h>   //stat()

#include "PalTime.h"
#include "Utils.h"

using namespace std;

typedef map<string, string> MonthMapT;

MonthMapT createMonthMap()
{
    MonthMapT m;
    m["Jan"] = "01";
    m["Feb"] = "02";
    m["Mar"] = "03";
    m["Apr"] = "04";
    m["May"] = "05";
    m["Jun"] = "06";
    m["Jul"] = "07";
    m["Aug"] = "08";
    m["Sep"] = "09";
    m["Oct"] = "10";
    m["Nov"] = "11";
    m["Dec"] = "12";
    return m;
}

MonthMapT gMonthMap(createMonthMap());

bool Utils::strIequal(const string &a, const string &b)
{
    return (a.size() == b.size() &&
            equal(a.begin(), a.end(), b.begin(), b.end(),
                  [](char c, char d)
                  {
                      return (c == d || tolower(c) == tolower(d));
                  }));
}

char *Utils::fromHexString(const string &hexStr, int &len)
{
    if (hexStr.empty())
        return 0;
    string s(hexStr);
    //prepare to tokenize the string into 1-byte hex values by inserting a space
    //after every 2 characters
    //if length is odd, insert the first space after the first character
    auto it = s.begin() + 2 - (s.length() % 2);
    //insert spaces
    while (it != s.end())
    {
        it = s.insert(it, ' ') + 3;
    }
    len = (hexStr.length() + 1)/2;  //size of the byte array
    char          *bytes = new char[len];
    char          *p     = bytes;
    unsigned int   val;
    istringstream  iss(s);
    //write every byte from the tokenized string
    while (iss >> std::hex >> val)
    {
        *p++ = val & 0xFF;
    }
    return bytes;
}

string Utils::fromHexString(const string &hexStr)
{
    if (hexStr.empty())
        return "";
    int len;
    char *bytes = fromHexString(hexStr, len);
    string outStr(bytes, len);
    delete [] bytes;
    return outStr;
}

wstring Utils::hex2Wstring(const string &hexStr)
{
    if (hexStr.empty())
        return L"";
    string s(hexStr);
    //prepare to tokenize the string into 1-byte or 2-byte hex values by
    //inserting a space after every 2 or 4 characters depending on the first
    //byte value,
    //if length is not a multiple of 2, pad it with a '0' prefix
    if (s.size() % 2 != 0)
        s.insert(0, "0");
    size_t pos = 0;
    unsigned int v;
    while (pos < s.size())
    {
        v = fromString<unsigned short>(s.substr(pos, 2));
        if (v != 0 && v <= 0x7F)
            pos += 2;
        else
            pos += 4;
        s.insert(pos++, " ");
    }
    wstring out;
    istringstream is(s);
    while (is >> std::hex >> v)
    {
        out.push_back(v);
    }
    return out;
}

u16string Utils::hex2u16string(const string &hexStr)
{
    string s(hexStr);
    //ensure length is multiple of 4
    if (s.size() % 4 != 0)
        s.resize(s.size() - (s.size() % 4));
    u16string out;
    if (!s.empty())
    {
        auto it = s.begin() + 4;
        //insert spaces
        while (it != s.end())
        {
            it = s.insert(it, ' ') + 5;
        }
        unsigned int v;
        istringstream is(s);
        while (is >> std::hex >> v)
        {
            out.push_back(v);
        }
    }
    return out;
}

string Utils::hexUtf16ToUtf8(const string &hexStr)
{
    if (hexStr.empty())
        return hexStr;
    u16string ustr(hex2u16string(hexStr));
#if defined(_WIN32) || defined(WIN32)
    wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
    const int16_t *p = reinterpret_cast<const int16_t *>(ustr.data());
    return toHexString(convert.to_bytes(p, p + ustr.size()));
#elif __GNUC__ >= 5 //linux
    wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
    return toHexString(convert.to_bytes(ustr));
#else
    return toHexString(ucs2ToUtf8(ustr));
#endif
}

string Utils::ucs2ToUtf8(u16string &ustr)
{
    size_t n = ustr.size();
    //allocate maximum possible number of UTF8 characters + NULL terminator
    char *buf = new char[(3 * n) + 1];
    char *bp = buf;
    char16_t *p = &ustr[0];
    for (; n>0; --n,++p)
    {
        if (*p <= 0x7F)
        {
            //plain single-byte ASCII
            *bp++ = (char) *p;
        }
        else if (*p <= 0x7FF)
        {
            //2 bytes
            *bp++ = 0xC0 | (*p >> 6);
            *bp++ = 0x80 | (*p & 0x3F);
        }
        else
        {
            //3 bytes
            *bp++ = 0xE0 | (*p >> 12);
            *bp++ = 0x80 | ((*p >> 6) & 0x3F);
            *bp++ = 0x80 | (*p & 0x3F);
        }
    }
    *bp = 0;
    string out(buf);
    delete [] buf;
    return out;
}

bool Utils::fromString(const string   &str,
                       vector<string> &data,
                       char            delimiter,
                       bool            acceptEmpty)
{
    if (str.empty())
        return false;
    size_t        oldSize = data.size();
    string        token;
    istringstream is(str);
    while (getline(is, token, delimiter))
    {
        if (acceptEmpty || !token.empty())
            data.push_back(token);
    }
    if (acceptEmpty && str[str.size() - 1] == delimiter)
        data.push_back(""); //str ends with delimiter - one empty last token
    return (data.size() > oldSize);
}

bool Utils::fromString(const string &str, set<string> &data, char delimiter)
{
    if (str.empty())
        return false;
    size_t        oldSize = data.size();
    string        token;
    istringstream is(str);
    while (getline(is, token, delimiter))
    {
        if (!token.empty())
            data.insert(token);
    }
    return (data.size() > oldSize);
}

int Utils::fromStringWithRange(const string &str,
                               set<int>     &data,
                               char          delimiter)
{
    if (str.empty())
        return 0;
    size_t        oldSize = data.size();
    int           min;
    int           max;
    char          c;
    string        token;
    istringstream is(str);
    while (getline(is, token, delimiter))
    {
        if (token.empty())
            continue;
        istringstream ist(token);
        if ((ist >> min) && min >= 0)
        {
            data.insert(min);
            if ((ist >> c >> max) && c == '-' && max > min)
            {
                for (++min; min<=max; ++min)
                {
                    data.insert(min);
                }
            }
        }
    }
    return (data.size() - oldSize);
}

string Utils::toStringWithRange(const set<int> &data,
                                const string   &delimiter,
                                const string   &header,
                                bool            splitLines)
{
    ostringstream os;
    if (!header.empty())
        os << header << "(" << data.size() << "): ";
    if (data.empty())
        return os.str();
    auto it = data.begin();
    int start = *it++;
    int end = start;
    int count = 0;
    for (; it!=data.end(); ++it)
    {
        if (*it == end + 1)
        {
            ++end;
            continue;
        }
        if (end == start)
        {
            os << start << delimiter;
            ++count;
        }
        else if (end == start + 1)
        {
            os << start << delimiter << end << delimiter;
            count += 2;
        }
        else
        {
            os << start << '-' << end << delimiter;
            count += 2;
        }
        if (splitLines && count >= 8)
        {
            os << '\n';
            count = 0;
        }
        start = *it;
        end = start;
    }
    if (end == start)
        os << start;
    else if (end == start + 1)
        os << start << delimiter << end;
    else
        os << start << '-' << end;
    return os.str();
}

string Utils::toStringWithRange(const vector<int> &data,
                                const string      &delimiter,
                                const string      &header,
                                bool               splitLines)
{
    return toStringWithRange(set<int>(data.begin(), data.end()), delimiter,
                             header, splitLines);
}

bool Utils::fromString(const string     &keys,
                       const string     &values,
                       map<int, string> &data,
                       char              delimiter,
                       bool              acceptEmpty)
{
    size_t        oldSize = data.size();
    string        key;
    string        val;
    istringstream iKeys(keys);
    istringstream iValues(values);
    while (getline(iKeys, key, delimiter) && getline(iValues, val, delimiter))
    {
        if (acceptEmpty || !val.empty())
            data[Utils::fromString<int>(key)] = val;
    }
    return (data.size() > oldSize);
}

string Utils::toString(double val, int precision)
{
    ostringstream os;
    if (precision <= 0)
        os << val;
    else
        os << std::fixed << setprecision(precision) << val;
    return os.str();
}

string Utils::toString(const map<string, string> &data,
                       const string              &delimiter)
{
    if (data.empty())
        return "";
    ostringstream os;
    auto it = data.begin();
    os << it->first << "=" << it->second;
    for (++it; it!=data.end(); ++it)
    {
        os << delimiter << it->first << "=" << it->second;
    }
    return os.str();
}

string Utils::toString(const wstring &wstr)
{
    return string(wstr.begin(), wstr.end());
}

time_t Utils::getTimeVal(const string &timestamp, const string &fmt)
{
    time_t result = 0;
    struct tm stm = {0};
    if (sscanf(timestamp.c_str(), fmt.c_str(), &stm.tm_year, &stm.tm_mon,
               &stm.tm_mday, &stm.tm_hour, &stm.tm_min, &stm.tm_sec) == 6)
    {
       stm.tm_year -= 1900; //years since 1900
       stm.tm_mon--;
       result = mktime(&stm);
    }
    return result;
}

string Utils::getTimeStr(const string &timestamp, const string &fmt)
{
    time_t result = getTimeVal(timestamp, fmt);
    return ((result > 0)? toString(result): "");
}

string Utils::formatTime(const struct tm &stm)
{
    char ts[sizeof("YYYY-MM-dd hh:mm:ss")];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &stm);
    return string(ts);
}

string Utils::formatTimeWithDayName(const struct tm &stm)
{
    char ts[sizeof("Day dd/MM/yyyy HH:mm:ss")];
    strftime(ts, sizeof(ts), "%a %d/%m/%Y %H:%M:%S", &stm);
    return string(ts);
}

string Utils::formatTime(time_t t, bool withDayName)
{
    struct tm stm;
    (void) PalTime::localtime(&stm, &t);
    return ((withDayName)? formatTimeWithDayName(stm): formatTime(stm));
}

string Utils::getTimestamp(bool formatted)
{
    struct tm stm;
    time_t msecs = PalTime::localtime(&stm);
    ostringstream oss;
    if (formatted)
    {
        oss << formatTime(stm);
    }
    else
    {
        char ts[sizeof("YYYYMMdd_hhmmss")];
        strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", &stm);
        oss << string(ts);
    }
    //append .millisecond
    oss << '.' << setfill('0') << setw(3) << msecs;
    return oss.str();
}

string Utils::getMsgTimestamp()
{
    struct tm stm;
    (void) PalTime::localtime(&stm);
    char ts[sizeof("dd/MM/YYYY hh:mm:ss")];
    strftime(ts, sizeof(ts), "%d/%m/%Y %H:%M:%S", &stm);
    return string(ts);
}

Utils::TimepointT Utils::getTimepoint(bool reset)
{
    return ((reset)? TimepointT{}: chrono::high_resolution_clock::now());
}

bool Utils::timepointValid(const TimepointT &t)
{
    return (chrono::duration_cast<chrono::milliseconds>
            (t.time_since_epoch()).count() > 0);
}

long Utils::timepointElapsedMs(const TimepointT &start)
{
    if (!timepointValid(start))
        return 0;
    return chrono::duration_cast<chrono::milliseconds>
           (chrono::high_resolution_clock::now() - start).count();
}

string Utils::getTransferStats(TimepointT start, int64_t sz)
{
    auto ms = chrono::duration<double, std::milli>(getTimepoint() - start)
              .count();
    double speed = (sz != 0 && ms > 0)? 8000 * ((sz/ms)/1024): 0; //kbps
    double intPart;
    ms = modf(ms, &intPart);
    long n = static_cast<long>(intPart);
    ostringstream oss;
    if (n >= 3600000) //1hour
    {
        oss << (n/3600000) << "h ";
        n = n % 3600000;
    }
    if (n >= 60000) //1min
    {
        oss << (n/60000) << "m ";
        n = n % 60000;
    }
    if (n >= 1000) //1sec
    {
        oss << (n/1000) << "s ";
        n = n % 1000;
    }
    ms += n;
    oss << ms << "ms";
    if (speed > 0)
    {
        oss << ", ";
        if (speed >= 1024)
            oss << speed/1024 << "Mbps / ";
        oss << speed << "kbps";
    }
    return oss.str();
}

string Utils::randomString(int minLen, int maxLen)
{
    if (minLen <= 0)
    {
        if (maxLen <= 0)
            return "";
        minLen = 1;
    }
    if (maxLen < minLen)
        maxLen = minLen;

    static const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
    //reuse maxLen to hold the output length, which is a random length in the
    //range if maxLen>minLen
    if (maxLen > minLen)
        maxLen = (rand() % (maxLen - minLen + 1)) + minLen;
    string str;
    while (maxLen-- > 0)
    {
        str.append(digits + (rand() % (sizeof(digits) - 1)), 1);
    }
    return str;
}

string Utils::scramble(int           init,
                       const string &data,
                       const string &key,
                       bool          appendKey)
{
    if (data.empty() || key.empty())
        return data;
    string s;
    auto ch = static_cast<char>(init);
    auto it = key.begin();
    for (auto c : data)
    {
        ch = (c + *it) ^ ch;
        s.append(1, ch);
        if (++it == key.end())
            it = key.begin();
    }
    if (appendKey)
        s.append(key);
    return s;
}

string Utils::toHexString(const char *data, int len, const string &sep)
{
    if (data == 0 || len == 0)
        return "";
    ostringstream oss;
    oss << setfill('0');
    oss << setw(2) << std::hex << std::uppercase
        << static_cast<unsigned int>(*data & 0xFF);
    for (--len, ++data; len>0; --len, ++data)
    {
        oss << sep << setw(2) << std::hex << std::uppercase
            << static_cast<unsigned int>(*data & 0xFF);
    }
    return oss.str();
}

string Utils::toHexString(const string &str, int len, const string &sep)
{
    return toHexString(str.c_str(), len, sep);
}

string Utils::toHexString(const string &str, const string &sep)
{
    return toHexString(str.c_str(), str.size(), sep);
}

string Utils::toHexString(unsigned int val, unsigned int len)
{
    ostringstream oss;
    if (len != 0)
        oss << setfill('0') << setw(len) << std::hex << std::uppercase << val;
    else
        oss << std::hex << std::uppercase << val;
    return oss.str();
}

size_t Utils::trim(string &str)
{
    if (str.empty())
        return 0;
    static const char whitespace[] = " \n\t\v\r\f";
    str.erase(0, str.find_first_not_of(whitespace));
    str.erase(str.find_last_not_of(whitespace) + 1);
    return str.length();
}

size_t Utils::removeQuotes(string &str)
{
    if (trim(str) >= 2 && str[0] == '\'')
    {
        size_t last = str.size() - 1;
        if (str[last] == '\'')
            str.erase(last, 1).erase(0, 1);
    }
    return str.length();
}

void Utils::makeUniqueFilepath(string &path, const string &extraPrefix)
{
    struct stat statBuf;
    if (stat(path.c_str(), &statBuf) != 0)
        return; //already unique
    string pre(path);
    string ext;
    auto pos = pre.find_last_of('.');
    if (pos != string::npos)
    {
        pre.erase(pos);
        ext = path.substr(pos);
    }
    //first try with just extraPrefix
    if (!extraPrefix.empty())
    {
        pre.append("-").append(extraPrefix);
        if (stat(path.assign(pre).append(ext).c_str(), &statBuf) != 0)
            return;
    }
    //still not unique - add number
    pre.append("-");
    int n = 1;
    do
    {
        path.assign(pre).append(toString(n++)).append(ext);
    }
    while (stat(path.c_str(), &statBuf) == 0);
}

time_t Utils::getUniqueRef()
{
    //start with epoch time relative to a base time (recent point within this
    //app's lifetime) to minimize range, instead of starting from a time_t over
    //1.6 billion - then just increment on every invocation
    //note: timestamp yyMMddhhmmss also achieves the main objective, but with
    //12-digit fixed length - this implementation gives <=9 digits for the next
    //30 years
    static time_t ref = time(0) - 1617235200; //base time 1/4/2021 00:00:00 UTC
    return ref++;
}
