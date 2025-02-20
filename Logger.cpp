/**
 * Implementation of a thread-safe logging class with file rollover.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2019. All Rights Reserved.
 *
 * @file
 * @version $Id: Logger.cpp 1191 2019-01-29 06:21:48Z zulzaidi $
 * @author Mohd Rozaimi
 * @author Zahari Hadzir
 */
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS  //prevent VS fopen() deprecated warning
#endif
#include <iomanip>        //setfill(), setw()
#include <iostream>       //cout
#include <set>
#include <assert.h>
#include <errno.h>
#include <stdio.h>        //fopen()
#include <sys/stat.h>     //stat()

#include "Locker.h"
#include "PalFiles.h"
#include "Utils.h"
#include "Logger.h"

#if defined(DEBUG) && defined(ENABLE_DEBUG_LOG)
#define L_DEFAULT L_DEBUG
#else
#define L_DEFAULT L_VERBOSE
#endif

#define MYERROR(msg) LOG(this, L_ERROR, msg)
#define MYINFO(msg)  LOG(this, L_INFO, msg)

using std::ostream;
using std::ostringstream;
using std::string;
using std::setfill;
using std::set;
using std::setw;

static Logger::LevelMapT createLevelMap()
{
    Logger::LevelMapT m;
#ifdef ENABLE_DEBUG_LOG
    m[Logger::L_DEBUG3]   = "TRACE3";
    m[Logger::L_DEBUG2]   = "TRACE2";
    m[Logger::L_DEBUG]    = "TRACE";
#endif
    m[Logger::L_VERBOSE]  = "VINFO";
    m[Logger::L_INFO]     = "INFO";
    m[Logger::L_WARNING]  = "WARNING";
    m[Logger::L_ERROR]    = "ERROR";
    m[Logger::L_DISABLED] = "DISABLED";
    return m;
}

//static initializers
Logger::LevelMapT Logger::sLevelMap(createLevelMap());

void *loggerStartWriterThread(void *arg)
{
    static_cast<Logger *>(arg)->writerThread();
    return 0;
}

Logger::Logger(const string &filename) :
mFp(stdout), mFilename("stdout"), mMaxFileSize(0),
mFileCount(0), mMaxFileCount(0), mIsRollingFile(false), mWriterThread(0),
mLevel(L_DEFAULT)
{
    init(filename);
}

Logger::Logger(const string &filename, const string &header) :
mFp(stdout), mFilename("stdout"), mHeader(header), mMaxFileSize(0),
mFileCount(0), mMaxFileCount(0), mIsRollingFile(false), mWriterThread(0),
mLevel(L_DEFAULT)
{
    init(filename);
}

Logger::~Logger()
{
    if (mWriterThread != 0)
    {
        while (!mDataQueue.empty())
            PalThread::msleep(50);
        PalSem::post(&mDataSem);
        PalThread::stop(mWriterThread);
    }
    if (mFp != stdout)
        fclose(mFp);
    PalLock::destroy(&mFileLock);
    PalLock::destroy(&mDataQueueLock);
    PalSem::destroy(&mDataSem);
}

void Logger::log(ostringstream &msgOss, LogLevel level)
{
    assert(level >= mLevel);
    checkStream(msgOss);
    //prefix log message with "timestamp LOGLEVEL"
    MsgData data = {(level == L_WARNING || level == L_ERROR),
                    "\n" + Utils::getTimestamp() + " " + sLevelMap[level] +
                    " " + msgOss.str()};
    PalLock::take(&mDataQueueLock);
    mDataQueue.push(data);
    PalSem::post(&mDataSem);
    PalLock::release(&mDataQueueLock);
}

void Logger::logRaw(ostringstream &msgOss)
{
    checkStream(msgOss);
    MsgData data = {false, msgOss.str()};
    PalLock::take(&mDataQueueLock);
    mDataQueue.push(data);
    PalSem::post(&mDataSem);
    PalLock::release(&mDataQueueLock);
}

#ifdef AGW
void Logger::logAsn(ostringstream         &msgOss,
                    asn_TYPE_descriptor_t *desc,
                    const void            *pdu,
                    LogLevel               level)
{
    if (mLevel <= level)
    {
        FILE *tmpf = tmpfile();
        asn_fprint(tmpf, desc, pdu);
        string str;
        str.resize(ftell(tmpf));
        rewind(tmpf);   //read from the start
        if (fread(&str[0], 1, str.size(), tmpf) > 0)
            msgOss << '\n' << str;
        fclose(tmpf);
        log(msgOss, level);
    }
}
#endif

int Logger::setFilename(const string &filename)
{
    if (filename == mFilename)
        return 0; //do nothing

    FILE *newFp = stdout;
    if (!filename.empty() && filename != "stdout")
    {
        //open file for appending
        newFp = fopen(filename.c_str(), "a");
        if (newFp == NULL)
        {
            MYERROR("Logger::setFilename: Cannot open log file " << filename
                    << " for writing.");
            return 1;
        }
        if (!mHeader.empty() && ftell(newFp) == 0)
            fprintf(newFp, "%s\n", mHeader.c_str());
    }

    if (newFp != mFp) //i.e. not changing from stdout to stdout
    {
        Locker lock(&mFileLock);
        string msg("\n" + Utils::getTimestamp() + " " + sLevelMap[L_INFO] +
                   " Logger:: Changed log from " + mFilename + " to " +
                   ((filename.empty())? "stdout": filename));
        fprintf(mFp, "%s\n", msg.c_str());
        fflush(mFp);
        if (mFp != stdout)
            fclose(mFp);
        mFp = newFp;
        if (newFp == stdout)
        {
            mFilename      = "stdout";
            mIsRollingFile = false;
        }
        else
        {
            mFilename = filename;
            //find the path and base name for rollover naming
            size_t pos = filename.find_last_of("/");
            if (pos != string::npos)
            {
                mFilenamePath = filename.substr(0, pos + 1);
                mFilenameBase = filename.substr(pos + 1);
            }
            else
            {
                mFilenamePath.clear();
                mFilenameBase = filename;
            }
            //find the extension
            pos = mFilenameBase.find_last_of(".");
            if (pos != string::npos)
            {
                mFilenameExt  = mFilenameBase.substr(pos);
                mFilenameBase = mFilenameBase.substr(0, pos);
            }
            else
            {
                mFilenameExt.clear();
            }
            mIsRollingFile = (mMaxFileSize > 0);
        }
    }
    return 0;
}

string Logger::getFilename() const
{
    return mFilename;
}

int Logger::setMaxFileSize(long size)
{
    if (size != 0 && size < MINIMUM_MAXFILESIZE)
    {
        MYERROR("Logger::setMaxFileSize: Invalid size (ignored): " << size
                << "\nSize cannot be less than " << MINIMUM_MAXFILESIZE
                << " bytes");
        return MINIMUM_MAXFILESIZE;
    }

    mMaxFileSize   = size;
    mIsRollingFile = (size > 0 && mFp != stdout);
    return 0;
}

long Logger::getMaxFileSize() const
{
    return mMaxFileSize;
}

int Logger::setMaxFileCount(int count)
{
    if (count < 0)
    {
        MYERROR("Logger::setMaxFileCount: Invalid maximum file count "
                "(ignored): " << count);
        return 1;
    }
    mMaxFileCount = count;
    return 0;
}

size_t Logger::getMaxFileCount()
{
    return mMaxFileCount;
}

void Logger::setLevel(LogLevel level)
{
    mLevel = level;
}

bool Logger::setLevel(const string &level)
{
    LogLevel newLevel = L_DEFAULT;
    if (!level.empty())
    {
        LevelMapT::iterator it = sLevelMap.begin();
        for (; it!=sLevelMap.end() && level!=it->second; ++it)
            ; //empty loop
        if (it == sLevelMap.end())
        {
            MYERROR("Logger::setLevel: Invalid log level (ignored): "
                    << level);
            return false;
        }
        newLevel = it->first;
    }
    if (newLevel != mLevel)
    {
        const string &old(sLevelMap[mLevel]);
        if (mLevel > L_INFO)
            mLevel = L_INFO;
        MYINFO("Logger:: Changing level from " << old << " to "
               << sLevelMap[newLevel]);
        mLevel = newLevel;
    }
    return true;
}

const string &Logger::getLevel() const
{
    return sLevelMap[mLevel];
}

const string &Logger::getDefaultLevel() const
{
    return sLevelMap.find(L_DEFAULT)->second;
}

string Logger::getValidLevels() const
{
    LevelMapT::iterator it = sLevelMap.begin();
    string s(it->second);
    for (++it; it!=sLevelMap.end(); ++it)
        s.append("|" + it->second);
    return s;
}

bool Logger::isEnabled(LogLevel level) const
{
    return (level != L_DISABLED && level >= mLevel);
}

void Logger::writerThread()
{
    string      msg;
    bool        printStdout = false;
    struct stat statBuf;

    for (;;)
    {
        if (!PalSem::wait(&mDataSem))
            continue;
        PalLock::take(&mDataQueueLock);
        if (mDataQueue.empty())
        {
            //semaphore posted with empty queue only at shutdown
            PalLock::release(&mDataQueueLock);
            break;
        }
        printStdout = mDataQueue.front().printStdout;
        msg         = mDataQueue.front().msg;
        mDataQueue.pop();
        PalLock::release(&mDataQueueLock);

        PalLock::take(&mFileLock);
        //if the file no longer exists, try to reopen it
        errno = 0;
        if ((mFp != stdout && stat(mFilename.c_str(), &statBuf) != 0 &&
             errno == ENOENT) ||
            (mFp == stdout && !mFilename.empty() && mFilename != "stdout"))
        {
            if (mFp != stdout)
                fclose(mFp);
            mFp = fopen(mFilename.c_str(), "a");
            if (mFp == NULL)
            {
                mFp = stdout;
                MYERROR("Logger:: Log file " << mFilename
                        << " was deleted or renamed by another process, "
                           "and could not be reopened for writing.");
            }
            else if (!mHeader.empty() && ftell(mFp) == 0)
            {
                fprintf(mFp, "%s\n", mHeader.c_str());
            }
        }
        fprintf(mFp, "%s\n", msg.c_str());
        fflush(mFp);
        if (mFp != stdout)
        {
            if (printStdout)
                std::cout << msg << std::endl;
            //roll over log file if necessary
            if (mIsRollingFile && mMaxFileSize <= ftell(mFp))
                rollover();
        }
        PalLock::release(&mFileLock);
    }
}

void Logger::init(const string &filename)
{
    PalSem::init(&mDataSem);
    PalLock::init(&mFileLock);
    PalLock::init(&mDataQueueLock);
    if (!filename.empty())
        setFilename(filename);
    PalThread::start(&mWriterThread, loggerStartWriterThread, this);
}

void Logger::rollover()
{
    const size_t  tsLen = sizeof("YYYYMMdd_hhmmss") - 1;
    ostringstream oss;
    //rename to "basename.timestamp.ext",
    //where timestamp is "YYYYMMdd_hhmmss"
    oss << mFilenamePath << mFilenameBase << '.'
        << Utils::getTimestamp(false).substr(0, tsLen) << mFilenameExt;
    fclose(mFp);
    int result = rename(mFilename.c_str(), oss.str().c_str());
    //if rename() fails, continue with the current file, and log later
    mFp = fopen(mFilename.c_str(), "a");
    //if fopen() fails, no choice but to write to stdout
    if (mFp == NULL)
        mFp = stdout;
    else if (!mHeader.empty() && ftell(mFp) == 0)
        fprintf(mFp, "%s\n", mHeader.c_str());

    if (result != 0)
    {
        ostringstream errOss;
        errOss << '\n' << Utils::getTimestamp() << ' ' << sLevelMap[L_ERROR]
               << " Logger::log: Failed to rename log file to";
        fprintf(mFp, "%s %s\n", errOss.str().c_str(), oss.str().c_str());
        fflush(mFp);
        return;
    }

    ++mFileCount;
    if (mMaxFileCount > 0 &&
        (mFileCount == 1 || mFileCount > mMaxFileCount))
    {
        //delete old files if necessary
        set<string> files;
        mFileCount = PalFiles::findFiles(mFilenamePath, mFilenameBase + ".",
                                         mFilenameExt, files);
        if (mFileCount > mMaxFileCount)
        {
            //delete the oldest files
            set<string>::iterator it = files.begin();
            size_t i = mFileCount - mMaxFileCount;
            for (; i>0; --i, ++it)
            {
                std::remove(string(mFilenamePath + *it).c_str());
            }
            mFileCount = mMaxFileCount;
        }
    } //if (mMaxFileCount > 0 ...
}

inline void Logger::checkStream(ostringstream &oss)
{
    //real world observation shows output of >100MB when something bad
    //happens - ~2MB should be a reasonable limit here
    if (!oss.good() || oss.tellp() > 2000000)
    {
        oss.str("");
        oss.clear();
        oss << "< ERROR in log message stream >";
    }
}
