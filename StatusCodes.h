/**
 * Status Codes manager to hold mapping of status codes to text and type.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: StatusCodes.h 1752 2023-09-12 06:36:20Z hazim.rujhan $
 * @author Zahari Hadzir
 */
#ifndef STATUSCODES_H
#define STATUSCODES_H

#include <map>
#include <string>
#include <time.h>   //time_t, time()

#include "MsgSp.h"
#include "PalLock.h"

class StatusCodes
{
public:
    enum
    {
        STATE_INVALID,
        STATE_DOWNLOADING,  //on client only
        STATE_READY
    };

    struct Data
    {
        Data() {}
        Data(int c, const std::string &t) : type(c), text(t) {}

        int         type;
        std::string text; //hexadecimal UTF8 Unicode
    };
    typedef std::map<int, Data>        DataMapT; //key is code
    //key is type, value is description in hexadecimal UTF8 Unicode
    typedef std::map<int, std::string> TypeMapT;

    /**
     * Initializes members.
     */
    static void init();

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
     * Processes a STATUS-DATA message.
     *
     * @param[in] msg The message.
     * @return true if the message needs to be passed up to user layer.
     */
    static bool processMsg(MsgSp *msg);

    /**
     * Gets the Status types. Overwrites existing content of the output
     * container.
     *
     * @param[out] types Status types.
     * @return true if successful and types not empty.
     */
    static bool getTypes(TypeMapT &types);

    /**
     * Gets the Status types for display.
     *
     * @return The Status types.
     */
    static std::string getTypes();

    /**
     * Sets a Status type.
     *
     * @param[in] type           The type.
     * @param[in] desc           The type description.
     * @param[in] doSetTimestamp true to re-set timestamp.
     */
    static void setType(int                type,
                        const std::string &desc,
                        bool               doSetTimestamp = true);

    /**
     * Gets a Status type's description.
     *
     * @param[in] type The type.
     * @return The description from sTypes, or empty string if not found.
     */
    static std::string getTypeDesc(int type);

    /**
     * Removes a Status type.
     *
     * @param[in] type The type.
     */
    static void removeType(int type);

    /**
     * Gets the complete Status data. Overwrites existing content of the output.
     *
     * @param[out] data Status data.
     * @return true if successful and data not empty.
     */
    static bool getData(DataMapT &data);

    /**
     * Gets all Status data and converts to string for display.
     *
     * @return The Status data.
     */
    static std::string getData();

    /**
     * Sets Status data.
     *
     * @param[in] code           Status Code.
     * @param[in] type           Status Type.
     * @param[in] text           Status Text.
     * @param[in] doSetTimestamp true to re-set timestamp.
     */
    static void setStatus(int                code,
                          int                type,
                          const std::string &text,
                          bool               doSetTimestamp = true);

    /**
     * Removes a Status code.
     *
     * @param[in] code Status Code.
     */
    static void removeStatus(int code);

    /**
     * Gets Status data for the given code.
     *
     * @param[in]  code Status Code.
     * @param[out] type Status Type.
     * @param[out] text Status Text as stored in DataMapT.
     * @return true if the Status Code exists.
     */
    static bool getStatus(int code, int &type, std::string &text);

    /**
     * Gets a Status code's text.
     *
     * @param[in] code Status Code.
     * @return Status Text from DataMapT, or empty string if not found.
     */
    static std::string getStatusText(int code);

    static void setStateReady();

    /**
     * Sets or clears STATE_DOWNLOADING.
     *
     * @param[in] doSet true to set the state, false to change
     *                  STATE_DOWNLOADING back to STATE_INVALID (e.g. when
     *                  disconnected during download).
     * @return true if state changed.
     */
    static bool setStateDownloading(bool doSet);

    static bool isValid() { return (sState != STATE_INVALID); }

    static bool isDownloading() { return (sState == STATE_DOWNLOADING); }

    static bool isReady() { return (sState == STATE_READY); }

private:
    static int            sState;
    static time_t         sTimestamp;  //last update time
    static TypeMapT       sTypes;
    static DataMapT       sData;
    static PalLock::LockT sDataLock;

    /**
     * On the server, sets the data update timestamp to the current time.
     * On the client, saves the data update timestamp from the received
     * server message.
     * Caller must be holding sDataLock.
     *
     * @param[in] msg The received message, if any. Used on the client only.
     */
    static void setTimestamp(MsgSp *msg = 0);
};
#endif //STATUSCODES_H
