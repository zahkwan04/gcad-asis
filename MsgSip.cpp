/**
 * Session Initiation Protocol (SIP) message implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: MsgSip.cpp 1704 2023-05-31 02:24:24Z rosnin $
 * @author Ahmad Syukri
 */
#include <sstream>
#include <assert.h>

#include "Utils.h"
#include "MsgSip.h"

using namespace std;

static const string SIP_PREFIX("SIP/");

//static initializers
const string     MsgSip::Value::ENDL("\r\n");
const string     MsgSip::Value::SIP_VERSION("SIP/2.0");
const string     MsgSip::Value::TERMINATOR("\r\n\r\n");
MsgSip::NameMapT MsgSip::sTypeNameMap(createTypeNameMap());
MsgSip::IdMapT   MsgSip::sTypeIdMap(createTypeIdMap());
MsgSip::NameMapT MsgSip::sFieldNameMap(createFieldNameMap());
MsgSip::IdMapT   MsgSip::sFieldIdMap(createFieldIdMap());
MsgSip::NameMapT MsgSip::sSubfieldNameMap(createSubfieldNameMap());
MsgSip::IdMapT   MsgSip::sSubfieldIdMap(createSubfieldIdMap());
MsgSip::NameMapT MsgSip::sStatusMap(createStatusMap());

MsgSip::MsgSip(int type, const string &requestUri) :
mType(type), mSeqId(0), mRespCode(Value::UNDEFINED), mReqUri(requestUri)
{
    if (sTypeNameMap.count(type) != 0)
        mStartLine.assign(sTypeNameMap[type]).append(" ").append(requestUri)
                  .append(" ").append(Value::SIP_VERSION);
}

MsgSip::MsgSip(int type, int status) :
mType(type), mSeqId(0), mRespCode(status)
{
    mStartLine.assign(Value::SIP_VERSION).append(" ")
              .append(Utils::toString(status)).append(" ");
    if (sStatusMap.count(status) != 0)
        mStartLine.append(sStatusMap[status]);
    else
        mStartLine.append("UNDEFINED");
}

MsgSip::MsgSip(const MsgSip &src) :
mType(src.mType), mSeqId(src.mSeqId), mRespCode(src.mRespCode),
mStartLine(src.mStartLine), mReqUri(src.mReqUri),
mContentBody(src.mContentBody),
mFields(src.mFields), mSubfields(src.mSubfields)
{
}

ostream &operator<<(ostream &os, const MsgSip &msg)
{
    return os << msg.toString("\n");
}

MsgSip &MsgSip::addField(int key, const string &value)
{
    mFields[key] = value;
    return *this;
}

MsgSip &MsgSip::addField(int key, int value)
{
    ostringstream os;
    os << value;
    mFields[key] = os.str();
    return *this;
}

MsgSip &MsgSip::addSubfield(int key, int subkey, const string &value)
{
    mSubfields[key][subkey] = value;
    if (mFields.count(key) == 0)
        mFields[key].assign(sSubfieldNameMap[Subfield::DIGEST]).append(" ");
    else
        mFields[key].append(1, Value::LIST_DELIMITER);
    mFields[key].append(sSubfieldNameMap[subkey]).
                 append(1, Value::PAIR_DELIMITER);
    string quotes;
    setQuotes(subkey, quotes);
    if (quotes.empty())
        mFields[key].append(value);
    else
        mFields[key].append(quotes).append(value).append(quotes);
    return *this;
}

MsgSip &MsgSip::removeField(int key)
{
    mFields.erase(key);
    return *this;
}

MsgSip &MsgSip::reset(int type)
{
    if (type >= 0)
        mType = type;
    mSeqId = 0;
    mRespCode = Value::UNDEFINED;
    mStartLine.clear();
    mReqUri.clear();
    mContentBody.clear();
    mFields.clear();
    mSubfields.clear();
    return *this;
}

string MsgSip::getFieldString(int key) const
{
    auto it = mFields.find(key);
    if (it != mFields.end())
        return it->second;
    return "";
}

int MsgSip::getFieldInt(int key) const
{
    int retVal = Value::UNDEFINED;
    auto it = mFields.find(key);
    if (it != mFields.end())
    {
        istringstream is(it->second);
        is >> retVal;
    }
    return retVal;
}

string MsgSip::getSubfieldString(int key, int subkey) const
{
    auto it = mSubfields.find(key);
    if (it != mSubfields.end())
    {
        auto it2 = it->second.find(subkey);
        if (it2 != it->second.end())
            return it2->second;
    }
    return "";
}

char *MsgSip::getBytes(int &len) const
{
    string msg(toString());
    len = msg.length();
    if (len == 0)
        return 0;
    char *bytes = new char[len];
    msg.copy(bytes, len);
    return bytes;
}

string MsgSip::toString(const string &endln) const
{
    ostringstream os;
    os << mStartLine << endln;

    for (const auto &it : mFields)
    {
        os << sFieldNameMap[it.first] << Value::FIELD_DELIMITER << ' '
           << it.second << endln;
    }
    os << endln;
    if (mContentBody.size() > 0)
        os << mContentBody;
    return os.str();
}

const string &MsgSip::getName() const
{
    if (sTypeNameMap.count(mType) == 0)
        return sTypeNameMap[Type::UNDEFINED];
    return sTypeNameMap[mType];
}

MsgSip &MsgSip::setSeqId(int seqId)
{
    mSeqId = seqId;
    ostringstream os;
    os << mSeqId << ' ' << getName();
    addField(MsgSip::Field::CSEQ, os.str());
    return *this;
}

int MsgSip::getReasonCause() const
{
    return getParamInt(Field::REASON, ";cause=");
}

int MsgSip::getParamInt(int key, const string &param) const
{
    int    val = Value::UNDEFINED;
    string s(getFieldString(key));
    if (!s.empty())
    {
        string p(param);
        if (p.back() != Value::PAIR_DELIMITER)
            p.append(1, Value::PAIR_DELIMITER);
        size_t pos = s.find(p);
        if (pos != string::npos)
        {
            s.erase(0, pos + p.length());
            istringstream is(s);
            if (getline(is, p, Value::PARAM_DELIMITER))
            {
                is.str(p);
                is >> val;
            }
        }
    }
    return val;
}

bool MsgSip::hasSubfieldHeader(int key) const
{
    return (mSubfields.count(key) != 0);
}

int MsgSip::getTypeId(const string &key)
{
    if (sTypeIdMap.count(key) == 0)
        return Type::UNDEFINED;
    return sTypeIdMap[key];
}

const string &MsgSip::getFieldName(int id)
{
    if (sFieldNameMap.count(id) == 0)
        return sFieldNameMap[Field::UNDEFINED];
    return sFieldNameMap[id];
}

int MsgSip::getFieldId(const string &key)
{
    if (sFieldIdMap.count(key) == 0)
        return Field::UNDEFINED;
    return sFieldIdMap[key];
}

int MsgSip::getSubfieldId(const string &key)
{
    if (sSubfieldIdMap.count(key) == 0)
        return Subfield::UNDEFINED;
    return sSubfieldIdMap[key];
}

#ifdef SIPTCP
MsgSip *MsgSip::parse(const string &str, int &contentLen)
#else
MsgSip *MsgSip::parse(const string &str)
#endif
{
#ifdef SIPTCP
    contentLen = 0;
#endif
    MsgSip *msg = new MsgSip();
    istringstream is(str);
    string val1;
    getline(is, val1);
    msg->mStartLine = val1;
    //difference between request and response message is the start line
    //  request:  <Method> <Request URI> <SIP Version>
    //            e.g. REGISTER sip:10.12.49.86 SIP/2.0
    //  response: <SIP Version> <Response Code> <Reason Phrase>
    //            e.g. SIP/2.0 401 Unauthorized
    istringstream iss(val1);
    string value;
    iss >> value;
    int id;
    //match the SIP_PREFIX part only, to handle different versions
    if (value.find(SIP_PREFIX) == 0)
    {
        if (!(iss >> id))
        {
            delete msg;
            return 0;
        }
        msg->setRespCode(id);
    }
    else
    {
        iss >> msg->mReqUri;
    }
    //each message has a header part and optionally a content body, separated by
    //a TERMINATOR - in UDP, the content body is included in str here, but in
    //TCP, it is not
    size_t pos = str.find(MsgSip::Value::TERMINATOR);
#ifndef SIPTCP
    if (pos != string::npos)
    {
        pos += Value::TERMINATOR.size();
        if (pos < str.size())
            msg->setContentBody(str.substr(pos));
    }
#endif
    id = Field::UNDEFINED;
    int n;
    string val2;
    while (getline(is, val1))
    {
        //the last line may be a blank line, i.e. with CRLF only,
        //on Linux this means a line with 1 character CR
        if (val1.size() <= 1)
            break;
        pos = val1.find(MsgSip::Value::FIELD_DELIMITER);
        if (pos == string::npos)
        {
            //continuation of previous field value, only if this starts with a
            //space or tab
            if (id != Field::UNDEFINED && (val1[0] == ' ' || val1[0] == '\t') &&
                Utils::trim(val1) != 0)
                msg->mFields[id].append(" ").append(val1);
            continue;
        }
        val1.replace(pos, 1, " ");
        istringstream isLine(val1);
        isLine >> val2;
        id = getFieldId(val2);
        getline(isLine, value);
        if (Utils::trim(value) == 0)
            continue;
        msg->mFields[id] = value;
        switch (id)
        {
            case Field::CSEQ:
            {
                isLine.clear();
                isLine.str(value);
                isLine >> msg->mSeqId >> value;
                msg->setType(getTypeId(value));
                break;
            }
            case Field::PROXY_AUTHENTICATE:
            case Field::WWW_AUTHENTICATE:
            {
                //parse value to subfield and its value
                //e.g. Digest realm="10.12.49.86",
                //nonce="7d686854-f293-4708-b34b-ab6492a7265f",
                //algorithm=MD5, qop="auth"
                val1.assign(sSubfieldNameMap[Subfield::DIGEST]).append(" ");
                pos = value.find(val1);
                if (pos != string::npos)
                    value.erase(0, pos + val1.length());
                isLine.clear();
                isLine.str(value);
                while (getline(isLine, value, Value::LIST_DELIMITER))
                {
                    pos = value.find(Value::PAIR_DELIMITER);
                    if (pos != string::npos)
                    {
                        val1 = value.substr(0, pos);
                        Utils::trim(val1);
                        val2 = value.substr(pos + 1);
                        n = Utils::trim(val2) - 1;
                        //remove double quotes at beginning and end of value
                        if (n > 0 && val2[0] == '\"' && val2[n] == '\"')
                            val2.erase(n, 1).erase(0, 1);
                        msg->mSubfields[id][getSubfieldId(val1)] = val2;
                    }
                }
                break;
            }
#ifdef SIPTCP
            case Field::CONTENT_LENGTH:
            {
                contentLen = Utils::fromString<int>(value);
                break;
            }
#endif
            default:
            {
                break;  //do nothing
            }
        }
    }
    return msg;
}

inline void MsgSip::setQuotes(int subfieldId, string &quotes) const
{
    switch (subfieldId)
    {
        case Subfield::CNONCE:
        case Subfield::NONCE:
        case Subfield::QOP:
        case Subfield::REALM:
        case Subfield::RESPONSE:
        case Subfield::URI:
        case Subfield::USERNAME:
            quotes.assign("\"");
            break;
        default:
            quotes.clear();
            break;
    }
}

MsgSip::NameMapT MsgSip::createTypeNameMap()
{
    NameMapT m;
    m[Type::ACK]       = "ACK";
    m[Type::BYE]       = "BYE";
    m[Type::CANCEL]    = "CANCEL";
    m[Type::INFO]      = "INFO";
    m[Type::INVITE]    = "INVITE";
    m[Type::MESSAGE]   = "MESSAGE";
    m[Type::NOTIFY]    = "NOTIFY";
    m[Type::OPTIONS]   = "OPTIONS";
    m[Type::PRACK]     = "PRACK";
    m[Type::PUBLISH]   = "PUBLISH";
    m[Type::REFER]     = "REFER";
    m[Type::REGISTER]  = "REGISTER";
    m[Type::SUBSCRIBE] = "SUBSCRIBE";
    m[Type::UNDEFINED] = "UNDEFINED";
    return m;
}

MsgSip::IdMapT MsgSip::createTypeIdMap()
{
    IdMapT m;
    for (const auto &it : sTypeNameMap)
    {
        m[it.second] = it.first;
    }
    return m;
}

MsgSip::NameMapT MsgSip::createFieldNameMap()
{
    NameMapT m;
    m[Field::ACCEPT]               = "Accept";
    m[Field::ACCEPT_ENCODING]      = "Accept-Encoding";
    m[Field::ACCEPT_LANGUAGE]      = "Accept-Language";
    m[Field::ALERT_INFO]           = "Alert-Info";
    m[Field::ALLOW]                = "Allow";
    m[Field::ALLOW_EVENTS]         = "Allow-Events";
    m[Field::AUTHENTICATION_INFO]  = "Authentication-Info";
    m[Field::AUTHORIZATION]        = "Authorization";
    m[Field::CALL_ID]              = "Call-ID";
    m[Field::CALL_INFO]            = "Call-Info";
    m[Field::CONTACT]              = "Contact";
    m[Field::CONTENT_DISP]         = "Content-Disposition";
    m[Field::CONTENT_ENCODING]     = "Content-Encoding";
    m[Field::CONTENT_LANGUAGE]     = "Content-Language";
    m[Field::CONTENT_LENGTH]       = "Content-Length";
    m[Field::CONTENT_TYPE]         = "Content-Type";
    m[Field::CSEQ]                 = "CSeq";
    m[Field::DATE]                 = "Date";
    m[Field::ERROR_INFO]           = "Error-Info";
    m[Field::EVENT]                = "Event";
    m[Field::EXPIRES]              = "Expires";
    m[Field::FROM]                 = "From";
    m[Field::IN_REPLY_TO]          = "In-Reply-To";
    m[Field::MAX_FORWARDS]         = "Max-Forwards";
    m[Field::MIME_VERSION]         = "MIME-Version";
    m[Field::MIN_EXPIRES]          = "Min-Expires";
    m[Field::ORGANIZATION]         = "Organization";
    m[Field::PRIORITY]             = "Priority";
    m[Field::PRIVACY]              = "Privacy";
    m[Field::PROXY_AUTHENTICATE]   = "Proxy-Authenticate";
    m[Field::PROXY_AUTHORIZATION]  = "Proxy-Authorization";
    m[Field::PROXY_REQUIRE]        = "Proxy-Require";
    m[Field::P_ASSERTED_IDENTITY]  = "P-Asserted-Identity";
    m[Field::P_PREFERRED_IDENTITY] = "P-Preferred-Identity";
    m[Field::RACK]                 = "RAck";
    m[Field::REASON]               = "Reason";
    m[Field::RECORD_ROUTE]         = "Record-Route";
    m[Field::REFERRED_BY]          = "Referred-By";
    m[Field::REFER_SUB]            = "Refer-Sub";
    m[Field::REFER_TO]             = "Refer-To";
    m[Field::REMOTE_PARTY_ID]      = "Remote-Party-ID";
    m[Field::REPLACES]             = "Replaces";
    m[Field::REPLY_TO]             = "Reply-To";
    m[Field::REQUEST_DISPOSITION]  = "Request-Disposition";
    m[Field::REQUIRE]              = "Require";
    m[Field::RETRY_AFTER]          = "Retry-After";
    m[Field::ROUTE]                = "Route";
    m[Field::RSEQ]                 = "RSeq";
    m[Field::SERVER]               = "Server";
    m[Field::SERVICE_ROUTE]        = "Service-Route";
    m[Field::SIP_ETAG]             = "SIP-ETag";
    m[Field::SIP_IF_MATCH]         = "SIP-If-Match";
    m[Field::SUBJECT]              = "Subject";
    m[Field::SUBSCRIPTION_STATE]   = "Subscription-State";
    m[Field::SUPPORTED]            = "Supported";
    m[Field::TIMESTAMP]            = "Timestamp";
    m[Field::TO]                   = "To";
    m[Field::UNSUPPORTED]          = "Unsupported";
    m[Field::USER_AGENT]           = "User-Agent";
    m[Field::USER_TO_USER]         = "User-To-User";
    m[Field::VIA]                  = "Via";
    m[Field::WARNING]              = "Warning";
    m[Field::WWW_AUTHENTICATE]     = "WWW-Authenticate";
    m[Field::X_FS_SUPPORT]         = "X-FS-Support";
    m[Field::UNDEFINED]            = "UNDEFINED";
    return m;
}

MsgSip::IdMapT MsgSip::createFieldIdMap()
{
    IdMapT m;
    for (const auto &it : sFieldNameMap)
    {
        m[it.second] = it.first;
    }
    return m;
}

MsgSip::NameMapT MsgSip::createSubfieldNameMap()
{
    NameMapT m;
    m[Subfield::ALGORITHM] = "algorithm";
    m[Subfield::CNONCE]    = "cnonce";
    m[Subfield::DIGEST]    = "Digest";
    m[Subfield::NC]        = "nc";
    m[Subfield::NONCE]     = "nonce";
    m[Subfield::QOP]       = "qop";
    m[Subfield::REALM]     = "realm";
    m[Subfield::RESPONSE]  = "response";
    m[Subfield::STALE]     = "stale";
    m[Subfield::URI]       = "uri";
    m[Subfield::USERNAME]  = "username";
    m[Subfield::UNDEFINED] = "UNDEFINED";
    return m;
}

MsgSip::IdMapT MsgSip::createSubfieldIdMap()
{
    IdMapT m;
    for (const auto &it : sSubfieldNameMap)
    {
        m[it.second] = it.first;
    }
    return m;
}

MsgSip::NameMapT MsgSip::createStatusMap()
{
    NameMapT m;
    m[Value::RESP_GFAIL_DECLINE]             = "Decline";
    m[Value::RESP_OK]                        = "OK";
    m[Value::RESP_PROV_RINGING]              = "Ringing";
    m[Value::RESP_PROV_TRYING]               = "Trying";
    m[Value::RESP_RFAIL_BAD_REQ]             = "Bad Request";
    m[Value::RESP_RFAIL_BUSY_HERE]           = "Busy Here";
    m[Value::RESP_RFAIL_METHOD_NOT_ALLOWED]  = "Method Not Allowed";
    m[Value::RESP_RFAIL_NOT_ACCEPTABLE_HERE] = "Not Acceptable Here";
    m[Value::RESP_RFAIL_REQ_TERMINATED]      = "Request Terminated";
    m[Value::RESP_RFAIL_TEMP_UNAVAILABLE]    = "Temporarily Unavailable";
    return m;
}
