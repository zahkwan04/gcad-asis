/**
 * A thread-safe logging class with file rollover capability.
 * Each log entry is time-stamped to the millisecond.
 * Rollover happens when the current file has exceeded the maximum size set
 * by the user (checked immediately after writing each entry).
 * The current file is saved with the rollover time stamp (to the minute)
 * appended to the filename, and a new file is created.
 * Usage:
 *   Call one of the level-specific logging macros LOGGER_<level>().
 *   The msg argument may include '<<' stream insertion operators but need
 *   not include std::endl.
 *   E.g. LOGGER_INFO(myLogger, "My log string " << myValue);
 *        LOGGER_ERROR(myLogger, "My log string " << somefunction()
 *                     << " more string");
 *   A Logger instance created on the heap (using 'new') must be deleted
 *   before exit to ensure proper closure of the log file.
 * A MOBILE build turns off all LOGGER macros and excludes all functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Logger.h 1671 2023-02-27 04:29:13Z rosnin $
 * @author Mohd Rozaimi
 * @author Zahari Hadzir
 */
#ifndef LOGGER_H
#define LOGGER_H

#ifdef MOBILE
#define LOGGER_DEBUG3(theLogger, msg)
#define LOGGER_DEBUG2(theLogger, msg)
#define LOGGER_DEBUG(theLogger, msg)
#define LOGGER_VERBOSE(theLogger, msg)
#define LOGGER_INFO(theLogger, msg)
#define LOGGER_WARNING(theLogger, msg)
#define LOGGER_ERROR(theLogger, msg)

#else //MOBILE
#include <fstream>
#include <map>
#include <queue>
#include <sstream>

#ifdef AGW
#include "constr_TYPE.h"    //asn_TYPE_descriptor_t
#endif
#include "PalLock.h"
#include "PalSem.h"
#include "PalThread.h"

#if defined(DEBUG) && !defined(ENABLE_DEBUG_LOG)
#define ENABLE_DEBUG_LOG
#endif

//the max file size should not be ridiculously small (except for testing),
//set any reasonable limit here
#define MINIMUM_MAXFILESIZE 2048 //bytes

#define LOG(theLoggerInstance, level, msg)             \
        do                                             \
        {                                              \
            if ((theLoggerInstance)->isEnabled(level)) \
            {                                          \
                std::ostringstream oss_;               \
                oss_ << msg;                           \
                (theLoggerInstance)->log(oss_, level); \
            }                                          \
        }                                              \
        while (false)

#define LOGGER_RAW(theLoggerInstance, msg)         \
        do                                         \
        {                                          \
            {                                      \
                std::ostringstream oss_;           \
                oss_ << msg;                       \
                (theLoggerInstance)->logRaw(oss_); \
            }                                      \
        }                                          \
        while (false)

#ifdef ENABLE_DEBUG_LOG
#define LOGGER_DEBUG3(theLogger, msg)  LOG(theLogger, Logger::L_DEBUG3,  msg)
#define LOGGER_DEBUG2(theLogger, msg)  LOG(theLogger, Logger::L_DEBUG2,  msg)
#define LOGGER_DEBUG(theLogger, msg)   LOG(theLogger, Logger::L_DEBUG,   msg)
#else
#define LOGGER_DEBUG3(theLogger, msg)
#define LOGGER_DEBUG2(theLogger, msg)
#define LOGGER_DEBUG(theLogger, msg)
#endif
#define LOGGER_VERBOSE(theLogger, msg) LOG(theLogger, Logger::L_VERBOSE, msg)
#define LOGGER_INFO(theLogger, msg)    LOG(theLogger, Logger::L_INFO,    msg)
#define LOGGER_WARNING(theLogger, msg) LOG(theLogger, Logger::L_WARNING, msg)
#define LOGGER_ERROR(theLogger, msg)   LOG(theLogger, Logger::L_ERROR,   msg)

#ifdef AGW
#define LOGGER_ASN(theLogger, msg, desc, pdu)                         \
        do                                                            \
        {                                                             \
            std::ostringstream ossAsn_;                               \
            ossAsn_ << msg;                                           \
            theLogger->logAsn(ossAsn_, desc, pdu, Logger::L_VERBOSE); \
        }                                                             \
        while (false)
#ifdef ENABLE_DEBUG_LOG
#define LOGGER_ASN2(theLogger, msg, desc, pdu)                       \
        do                                                           \
        {                                                            \
            std::ostringstream ossAsn_;                              \
            ossAsn_ << msg;                                          \
            theLogger->logAsn(ossAsn_, desc, pdu, Logger::L_DEBUG2); \
        }                                                            \
        while (false)
#else
#define LOGGER_ASN2(theLogger, msg, desc, pdu)
#endif //ENABLE_DEBUG_LOG
#endif //AGW

class Logger
{
public:
    /**
     * Log levels.
     * Logger is set to a particular (system-wide) level, and subsequently
     * all log messages with a lower level than that are discarded.
     * E.g. if the current level is L_ERROR, then debug, info and warning
     * messages will not be logged. If it is L_DISABLED, nothing will be
     * logged.
     */
    enum LogLevel
    {
        L_DEBUG3,
        L_DEBUG2,
        L_DEBUG,
        L_VERBOSE,
        L_INFO,
        L_WARNING,
        L_ERROR,
        L_DISABLED
    };

    typedef std::map<LogLevel, std::string> LevelMapT;

    /**
     * Constructor.
     *
     * @param[in] filename The log filename. Omit to log to standard output.
     */
    Logger(const std::string &filename = "");

    /**
     * Constructor with a file header line.
     *
     * @param[in] filename The log filename.
     * @param[in] header   The header line.
     */
    Logger(const std::string &filename, const std::string &header);

    /**
     * Destructor closes the log file, if one is open.
     */
    ~Logger();

    /**
     * Writes a log message.
     *
     * @param[in] msgOss A string stream containing the message.
     * @param[in] level  The log message level. Defaults to L_INFO if not
     *                   provided.
     */
    void log(std::ostringstream &msgOss, LogLevel level = L_INFO);

    /**
     * Writes a raw log message without adding the timestamp and level.
     *
     * @param[in] msgOss A string stream containing the message.
     */
    void logRaw(std::ostringstream &msgOss);

#ifdef AGW
    /**
     * Logs the content of an A-CAPI PDU.
     *
     * @param[in] msgOss A string stream containing the message beginning.
     * @param[in] desc   The PDU type descriptor.
     * @param[in] pdu    The PDU.
     * @param[in] level  The log level.
     */
    void logAsn(std::ostringstream    &msgOss,
                asn_TYPE_descriptor_t *desc,
                const void            *pdu,
                LogLevel               level = L_DEBUG);
#endif

/**
     * Sets the log destination.
     * If the destination is not stdout, opens the file (new or existing) for
     * appending. Closes the previously opened file, if any.
     *
     * @param[in] filename The log filename. Empty string or "stdout" to log
     *                     to standard output. A filename that is the same as
     *                     the current filename is ignored without error.
     * @return 0 if successful, non-zero if the file cannot be opened.
     */
    int setFilename(const std::string &filename);

    /**
     * Gets the current log destination.
     *
     * @return The current log filename, or "stdout".
     */
    std::string getFilename() const;

    /**
     * Sets the maximum log file size.
     *
     * @param size The maximum log file size in bytes.
     *             0 means no limit. Otherwise must be >=MINIMUM_MAXFILESIZE.
     * @return 0 if successful, the minimum size if the size is too small.
     */
    int setMaxFileSize(long size);

    /**
     * Gets the maximum log file size.
     *
     * @return The maximum log file size in bytes.
     */
    long getMaxFileSize() const;

    /**
     * Sets the maximum number of archived (renamed) log files.
     * The oldest files are deleted automatically.
     *
     * @param count The maximum number of log files. 0 means no limit - no
     *              files are deleted.
     * @return 0 if successful, non-zero for an invalid number.
     */
    int setMaxFileCount(int count);

    /**
     * Gets the maximum number of archived (renamed) log files.
     *
     * @return The maximum number of log files.
     */
    size_t getMaxFileCount();

    /**
     * Sets the log level by value.
     *
     * @param[in] level The log level.
     */
    void setLevel(LogLevel level);

    /**
     * Sets the log level by name.
     *
     * @param[in] level The case-sensitive log level name, or empty string to
     *                  set the default level.
     * @return true if level is valid.
     */
    bool setLevel(const std::string &level);

    /**
     * Gets the current log level.
     *
     * @return The current log level name.
     */
    const std::string &getLevel() const;

    /**
     * Gets the default log level.
     *
     * @return The default log level name.
     */
    const std::string &getDefaultLevel() const;

    /**
     * Gets the valid log level names to be presented in a usage string.
     *
     * @return A string showing all valid log level names separated by '|'.
     */
    std::string getValidLevels() const;

    /**
     * Checks whether the given log level is currently enabled based on the
     * system-wide log level.
     *
     * @param[in] level The log level.
     * @return true if enabled.
     */
    bool isEnabled(LogLevel level) const;

    /**
     * Thread that does the actual writing to file.
     */
    void writerThread();

private:
    struct MsgData
    {
        bool        printStdout;        ///< true to print to stdout too
        std::string msg;                ///< the log message
    };

    FILE             *mFp;              ///< log file pointer
    std::string       mFilename;        ///< log filename
    std::string       mFilenamePath;    ///< file path incl. the last '/'
    std::string       mFilenameBase;    ///< file base name
    std::string       mFilenameExt;     ///< file extension incl. '.'
    std::string       mHeader;          ///< file header, if needed
    long              mMaxFileSize;     ///< in bytes
    /// the current number of archived log files.
    size_t            mFileCount;
    size_t            mMaxFileCount;    ///< maximum num. of archived files
    bool              mIsRollingFile;   ///< true if rollover enabled
    PalSem::SemT      mDataSem;         ///< signals data posting
    std::queue<MsgData> mDataQueue;

    PalThread::ThreadT mWriterThread;
    PalLock::LockT    mFileLock;        ///< guards file access
    PalLock::LockT    mDataQueueLock;   ///< guards data queue access
    LogLevel          mLevel;           ///< current log level

    static LevelMapT  sLevelMap;        ///< level value-to-string map

    Logger(const Logger &);
    Logger &operator=(const Logger &);

    /**
     * Initializes some members.
     *
     * @param[in] filename The log filename. Empty string if using stdout.
     */
    void init(const std::string &filename);

    /**
     * Rolls over the log file by renaming it and opening a new file with the
     * original name.
     */
    void rollover();

    /**
     * Checks whether an output stream has error or excessively large
     * content. If so, clears all errors and replaces the content with a
     * short error string.
     *
     * @param[in,out] oss The output stream.
     */
    void checkStream(std::ostringstream &oss);
};
#endif //MOBILE
#endif //LOGGER_H
