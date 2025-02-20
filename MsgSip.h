/**
 * Session Initiation Protocol (SIP) message description.
 * Sample message:@code
 * INVITE sip:1000@10.12.49.86 SIP/2.0
 * Allow:
 * INVITE,ACK,BYE,CANCEL,OPTIONS,PRACK,REFER,NOTIFY,SUBSCRIBE,INFO,MESSAGE
 * Call-ID: rqrzvvuzaq@ubuntu
 * Contact: <sip:1007@10.12.49.71>
 * Content-Type: application/sdp
 * CSeq: 4 INVITE
 * From: "1007" <sip:1007@10.12.49.86>;tag=clcvzaxbig
 * Max-Forwards: 70
 * Proxy-Authorization: Digest
 * username="1007",realm="10.12.49.86",nonce="9361076b-12b1-43b9-9221-
 * 3d07582aca48",uri="sip:10.12.49.86",
 * response="31346991c7dd499db4af26311ae62835",algorithm=MD5,
 * cnonce="d1fa007348",qop=auth,nc=00000001
 * Supported: replaces,norefersub,100rel
 * To: <sip:1000@10.12.49.86>
 * User-Agent: SCAD/0.0.1
 * Via: SIP/2.0/UDP 10.12.49.71;rport;branch=z9hG4bKmiyqjsuc
 * Content-Length: 333
 *
 * v=0
 * o=twinkle 1950318061 2004287325 IN IP4 10.12.49.71
 * s=-
 * c=IN IP4 10.12.49.71
 * t=0 0
 * m=audio 8000 RTP/AVP 98 97 96 8 0 3 101
 * a=rtpmap:98 speex/16000
 * a=rtpmap:97 speex/8000
 * a=rtpmap:96 iLBC/8000
 * a=rtpmap:8 PCMA/8000
 * a=rtpmap:0 PCMU/8000
 * a=rtpmap:3 GSM/8000
 * a=rtpmap:101 telephone-event/8000
 * a=fmtp:101 0-15
 * a=ptime:20
 * @endcode
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2022. All Rights Reserved.
 *
 * @file
 * @version $Id: MsgSip.h 1612 2022-05-23 05:59:35Z rosnin $
 * @author Ahmad Syukri
 */
#ifndef MSGSIP_H
#define MSGSIP_H

#include <map>
#include <string>

#include "Utils.h"

class MsgSip
{
public:
    class Type
    {
    public:
        enum
        {
            ACK,
            BYE,
            CANCEL,
            INFO,
            INVITE,
            MESSAGE,
            NOTIFY,
            OPTIONS,
            PRACK,
            PUBLISH,
            REFER,
            REGISTER,
            SUBSCRIBE,
            UNDEFINED
        };
    }; //class Type

    class Field
    {
    public:
        enum
        {
            //the first few follow RFC-recommended order
            VIA,
            ROUTE,
            RECORD_ROUTE,
            PROXY_REQUIRE,
            MAX_FORWARDS,
            PROXY_AUTHORIZATION,
            FROM,
            TO,
            CALL_ID,
            CSEQ,
            CONTACT,
            ACCEPT,
            ACCEPT_ENCODING,
            ACCEPT_LANGUAGE,
            ALERT_INFO,
            ALLOW,
            ALLOW_EVENTS,
            AUTHENTICATION_INFO,
            AUTHORIZATION,
            CALL_INFO,
            CONTENT_DISP,
            CONTENT_ENCODING,
            CONTENT_LANGUAGE,
            DATE,
            ERROR_INFO,
            EVENT,
            EXPIRES,
            IN_REPLY_TO,
            MIME_VERSION,
            MIN_EXPIRES,
            ORGANIZATION,
            PRIORITY,
            PRIVACY,
            PROXY_AUTHENTICATE,
            P_ASSERTED_IDENTITY,
            P_PREFERRED_IDENTITY,
            RACK,
            REASON,
            REFERRED_BY,
            REFER_SUB,
            REFER_TO,
            REMOTE_PARTY_ID,
            REPLACES,
            REPLY_TO,
            REQUEST_DISPOSITION,
            REQUIRE,
            RETRY_AFTER,
            RSEQ,
            SERVER,
            SERVICE_ROUTE,
            SIP_ETAG,
            SIP_IF_MATCH,
            SUBJECT,
            SUBSCRIPTION_STATE,
            SUPPORTED,
            TIMESTAMP,
            UNSUPPORTED,
            USER_AGENT,
            USER_TO_USER,
            WARNING,
            WWW_AUTHENTICATE,
            X_FS_SUPPORT,
            CONTENT_TYPE,
            CONTENT_LENGTH,
            UNDEFINED
        };
    }; //class Field

    class Subfield
    {
    public:
        enum
        {
            ALGORITHM,
            CNONCE,
            DIGEST,
            NC,
            NONCE,
            QOP,
            REALM,
            RESPONSE,
            STALE,
            URI,
            USERNAME,
            UNDEFINED
        };
    }; //class Subfield


    class Value
    {
    public:
        enum
        {
            //Response codes:
            //1xx: Provisional - request received, continuing to process
            //     the request
            //2xx: Success - the action was successfully received,
            //     understood, and accepted
            //3xx: Redirection - further action needs to be taken in order
            //     to complete the request
            //4xx: Request Failure - the request contains bad syntax or
            //     cannot be fulfilled at this server
            //5xx: Server Failure - the server failed to fulfill an
            //     apparently valid request
            //6xx: Global Failure - the request cannot be fulfilled at any
            //     server
            RESP_PROV_TRYING               = 100,
            RESP_PROV_RINGING              = 180,
            RESP_OK                        = 200,
            RESP_RFAIL_BAD_REQ             = 400,
            RESP_RFAIL_UNAUTHORIZED        = 401,
            RESP_RFAIL_NOT_FOUND           = 404,
            RESP_RFAIL_METHOD_NOT_ALLOWED  = 405,
            RESP_RFAIL_PROXY_AUTH_REQD     = 407,
            RESP_RFAIL_TEMP_UNAVAILABLE    = 480,
            RESP_RFAIL_BUSY_HERE           = 486,
            RESP_RFAIL_REQ_TERMINATED      = 487,
            RESP_RFAIL_NOT_ACCEPTABLE_HERE = 488,
            RESP_RFAIL_SVC_UNAVAILABLE     = 503,
            RESP_GFAIL_DECLINE             = 603,

            PARAM_REASON_CAUSE_CALL_COMPLETED_ELSEWHERE = 200
        };
        static const int  UNDEFINED       = -1;
        static const char FIELD_DELIMITER = ':';  //between field and value
        static const char PAIR_DELIMITER  = '=';  //between tag and value
        static const char PARAM_DELIMITER = ';';  //between param=value pairs
        static const char LIST_DELIMITER  = ',';  //inside list of values

        static const std::string ENDL;
        static const std::string SIP_VERSION;
        static const std::string TERMINATOR;
    }; //class Value

    MsgSip(): mType(Type::UNDEFINED), mSeqId(0), mRespCode(Value::UNDEFINED) {}

    /**
     * Constructor for request.
     *
     * @param[in] type       The message type.
     * @param[in] requestUri The request URI.
     */
    MsgSip(int type, const std::string &requestUri);

    /**
     * Constructor for response.
     *
     * @param[in] type   The message type.
     * @param[in] status The status code.
     */
    MsgSip(int type, int status);

    /**
     * Copy constructor.
     *
     * @param[in] src The source.
     */
    MsgSip(const MsgSip &src);

    friend std::ostream &operator<<(std::ostream &os, const MsgSip &msg);

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @return *this.
     */
    MsgSip &addField(int key, const std::string &value);

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @return *this.
     */
    MsgSip &addField(int key, int value);

    /**
     * Adds a message subfield.
     *
     * @param[in] key    The field key.
     * @param[in] subkey The subfield key.
     * @param[in] value  The subfield value.
     * @return *this.
     */
    MsgSip &addSubfield(int key, int subkey, const std::string &value);

    /**
     * Adds a message field.
     *
     * @param[in] key   The field key.
     * @param[in] value The field value.
     * @tparam    T     The value type.
     * @return *this.
     */
    template<class T>
    MsgSip &addField(int key, T value);

    /**
     * Removes a field.
     *
     * @param[in] key The field key. Does nothing if the key does not exist.
     * @return *this.
     */
    MsgSip &removeField(int key);

    /**
     * Clears the message data, and optionally changes the type.
     *
     * @param[in] type The new type, if any.
     * @return *this.
     */
    MsgSip &reset(int type = -1);

    /**
     * Gets a field string.
     *
     * @param[in] key The field key.
     * @return Field string if the key exists, otherwise empty string.
     */
    std::string getFieldString(int key) const;

    /**
     * Gets a field numeric value.
     *
     * @param[in] key The field key.
     * @return Field value if the key exists and has a numeric value,
     *         otherwise Field::UNDEFINED.
     */
    int getFieldInt(int key) const;

    /**
     * Gets a field numeric value.
     *
     * @param[in]  key   The field key.
     * @param[out] value The value if found, otherwise not modified.
     * @tparam     T     The value type.
     * @return true if value found.
     */
    template<class T>
    bool getFieldVal(int key, T &value) const;

    /**
     * Gets a subfield string.
     *
     * @param[in] key    The field key.
     * @param[in] subkey The subfield key.
     * @return Subfield string if the keys exist, otherwise empty string.
     */
    std::string getSubfieldString(int key, int subkey) const;

    /**
     * Does message serialization and gets the actual data bytes.
     *
     * @param[out] len The number of data bytes.
     * @return The data bytes. Caller must destroy with delete [].
     */
    char *getBytes(int &len) const;

    /**
     * Converts a class object to string.
     *
     * @param[in] endln The line terminator string.
     * @return The string representation of the object.
     */
    std::string toString(const std::string &endln = Value::ENDL) const;

    /**
     * Gets the message name.
     *
     * @return The message name if exists, otherwise "UNDEFINED".
     */
    const std::string &getName() const;

    MsgSip &setType(int type)
    {
        mType = type;
        return *this;
    }

    int getType() const { return mType; }

    MsgSip &setSeqId(int seqId);

    int getSeqId() const { return mSeqId; }

    const std::string &getReqUri() const { return mReqUri; }

    MsgSip &setContentBody(const std::string &contentBody)
    {
        mContentBody = contentBody;
        return *this;
    }

    const std::string &getContentBody() const { return mContentBody; }

    /**
     * Sets response code of the message.
     *
     * @param[in] code The response code.
     */
    void setRespCode(int code) { mRespCode = code; }

    /**
     * Gets message response code.
     *
     * @return The message response code.
     */
    int getRespCode() const { return mRespCode; }

    /**
     * Gets the 'cause' parameter value of REASON field.
     * E.g. Reason: SIP;cause=200;text="Call completed elsewhere"
     *
     * @return The value, or Value::UNDEFINED if not present.
     */
    int getReasonCause() const;

    /**
     * Gets a parameter value of a particular field.
     * E.g.: Via: SIP/2.0/UDP 10.11.12.13:5060;rport=51062;branch=...
     *       - key=Field::VIA, param=";rport"
     *       Reason: SIP;cause=200;text="Call completed elsewhere"
     *       - key=Field::REASON, param=";cause="
     *
     * @param[in] key   The field ID.
     * @param[in] param The parameter. May start with ';' and end with '='.
     * @return The value, or Value::UNDEFINED if not present.
     */
    int getParamInt(int key, const std::string &param) const;

    /**
     * Checks existence of a field that contains subfields.
     *
     * @param[in] key The field ID.
     * @return true if exists.
     */
    bool hasSubfieldHeader(int key) const;

    /**
     * Gets a type ID using its name.
     *
     * @param[in] key The type name.
     * @return Type ID if the key exists, otherwise Type::UNDEFINED.
     */
    static int getTypeId(const std::string &key);

    /**
     * Gets a field name.
     *
     * @param[in] id The field ID.
     * @return The field name if the ID exists, otherwise "UNDEFINED".
     */
    static const std::string &getFieldName(int id);

    /**
     * Gets a field ID using its name.
     *
     * @param[in] key The field name.
     * @return Field ID if the key exists, otherwise Field::UNDEFINED.
     */
    static int getFieldId(const std::string &key);

    /**
     * Gets a subfield ID using its name.
     *
     * @param[in] key The subfield name.
     * @return Subfield ID if the key exists,
     *         otherwise Subfield::UNDEFINED.
     */
    static int getSubfieldId(const std::string &key);

    /**
     * Parses a message string into a message object.
     *
     * @param[in]  str        The message string.
     * @param[out] contentLen The content body length.
     * @return The message object, or 0 on failure. Caller takes ownership
     *         of the created object, and is responsible for deleting it.
     */
#ifdef SIPTCP
    static MsgSip *parse(const std::string &str, int &contentLen);
#else
    static MsgSip *parse(const std::string &str);
#endif

private:
    typedef std::map<int, std::string> FieldsMapT;
    typedef std::map<int, FieldsMapT>  SubfieldsMapT;
    typedef std::map<int, std::string> NameMapT;
    typedef std::map<std::string, int> IdMapT;

    int           mType;        //message type
    int           mSeqId;       //message sequence ID
    int           mRespCode;    //response code from server
    std::string   mStartLine;   //start line values
    std::string   mReqUri;      //request URI
    std::string   mContentBody; //content values
    FieldsMapT    mFields;      //field values
    SubfieldsMapT mSubfields;   //subfield values

    static NameMapT sTypeNameMap;     //message type names
    static IdMapT   sTypeIdMap;       //message type IDs
    static NameMapT sFieldNameMap;    //field names
    static IdMapT   sFieldIdMap;      //field IDs
    static NameMapT sSubfieldNameMap; //subfield names
    static IdMapT   sSubfieldIdMap;   //subfield IDs
    static NameMapT sStatusMap;       //status reason phrases

    /**
     * Determines if a subfield needs double quotes.
     *
     * @param[in]  subfieldId The subfield ID.
     * @param[out] quotes     "\"" or empty string.
     */
    void setQuotes(int subfieldId, std::string &quotes) const;

    /**
     * Creates a mapping of type ID to string.
     *
     * @return The map.
     */
    static NameMapT createTypeNameMap();

    /**
     * Creates a mapping of type string to ID.
     *
     * @return The map.
     */
    static IdMapT createTypeIdMap();

    /**
     * Creates a mapping of field ID to string.
     *
     * @return The map.
     */
    static NameMapT createFieldNameMap();

    /**
     * Creates a mapping of field names to ID.
     *
     * @return The map.
     */
    static IdMapT createFieldIdMap();

    /**
     * Creates a mapping of value ID to string.
     *
     * @return The map.
     */
    static NameMapT createSubfieldNameMap();

    /**
     * Creates a mapping of value names to ID.
     *
     * @return The map.
     */
    static IdMapT createSubfieldIdMap();

    /**
     * Creates a mapping of status code to reason phrase.
     *
     * @return The map.
     */
    static NameMapT createStatusMap();
};

template<class T>
MsgSip &MsgSip::addField(int key, T value)
{
    mFields[key] = Utils::toString(value);
    return *this;
}

template<class T>
bool MsgSip::getFieldVal(int key, T &value) const
{
    FieldsMapT::const_iterator it = mFields.find(key);
    return (it != mFields.end() &&
            Utils::fromString<T>(it->second, value));
}
#endif //MSGSIP_H
