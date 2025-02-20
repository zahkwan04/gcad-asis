/**
 * @file
 * @version $Id: testclient.cpp 1647 2022-10-05 03:18:44Z zulzaidi $
 */
#include <algorithm>    //count
#include <fstream>
#include <iomanip>      //setw, setfill
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>     //getpid()

#include "Logger.h"
#include "MsgSp.h"
#include "PalSem.h"
#include "PalSocket.h"      //just for finalize()
#include "ServerSession.h"
#include "StatusCodes.h"
#include "SubsData.h"
#include "Utils.h"
#include "version.h"

using namespace std;

static const string USERNAMEBASE("0");
static const int    MAXCLIENTS = 1000;
//dummy gps data
static const string GPSDATA[] =
{
    "$GPGLL,0311.334320,N,10144.076881,E,041117.346,A,A*5C\x0D\x0A",
    "$GPGLL,0311.312190,N,10144.079237,E,061746.948,A,A*5D\x0D\x0A",
    "$GPGLL,0311.347665,N,10144.040622,E,061816.752,A,A*55\x0D\x0A",
    "$GPGLL,0311.305405,N,10144.079549,E,061817.668,A,A*5A\x0D\x0A",
    "$GPGLL,0311.331612,N,10144.055009,E,061916.144,A,A*5F\x0D\x0A",
    "$GPGLL,0311.352741,N,10144.047504,E,061917.061,A,A*51\x0D\x0A",
    "$GPGLL,0311.338758,N,10144.060096,E,061944.816,A,A*50\x0D\x0A",
    "$GPGLL,0311.350246,N,10144.053022,E,061947.781,A,A*59\x0D\x0A",
    "$GPGLL,0311.343642,N,10144.043083,E,062015.537,A,A*53\x0D\x0A",
    "$GPGLL,0311.349071,N,10144.051933,E,062017.477,A,A*59\x0D\x0A",
    "$GPGLL,0311.347748,N,10144.036910,E,062046.257,A,A*5A\x0D\x0A",
    "$GPGLL,0311.343284,N,10144.060769,E,062047.173,A,A*5C\x0D\x0A",
    "$GPGLL,0311.353153,N,10144.032264,E,062116.977,A,A*52\x0D\x0A",
    "$GPGLL,0311.341356,N,10144.057250,E,062117.893,A,A*58\x0D\x0A",
    "$GPGLL,0311.333318,N,10144.052840,E,062144.625,A,A*5C\x0D\x0A",
    "$GPGLL,0311.340526,N,10144.056592,E,062147.589,A,A*53\x0D\x0A",
    "$GPGLL,0311.343711,N,10144.055256,E,062217.285,A,A*57\x0D\x0A",
    "$GPGLL,0311.340673,N,10144.059694,E,062246.981,A,A*5C\x0D\x0A",
    "$GPGLL,0311.344465,N,10144.033988,E,062316.785,A,A*5D\x0D\x0A",
    "$GPGLL,0311.362333,N,10144.044382,E,062317.701,A,A*50\x0D\x0A"
};
static const int GPSDATA_COUNT = sizeof(GPSDATA)/sizeof(GPSDATA[0]);

static int           gFleetId     = 1;
static Logger       *gLogger      = 0;
//to prevent signaling after gSignalSem is destroyed
static bool          gSignallable = false;
static PalSem::SemT  gSignalSem;

//main subscriber data containing all VPNs, Divisions, Fleets, Groups and
//SSIs
SubsData::IdMapT       gSubsDataMap;
//all types of group in each fleet
SubsData::IdMapT       gFleet2GrpsMap;
//description of each SSI
SubsData::Ssi2DescMapT gSsi2DescMap;

//forward declare
class Client;
void serverMsg(Client *cl, MsgSp *msg);

typedef map<int, Client *> ClientsMapT;

class Client
{
public:
    //callback signature to receive a server message.
    typedef void (*RecvCallbackFn)(Client *cl, MsgSp *msg);

    Client(int id, RecvCallbackFn cbFn, bool doSubsData = true) :
    mId(id), mCallId(0), mCallSsi(0), mMonGssi(0), mCbFn(cbFn)
    {
        ostringstream os;
        os << USERNAMEBASE << setw(2) << setfill('0') << gFleetId << mId;
        mUserId = os.str();
        mSs = new ServerSession(mUserId, "password", "-", this, ssCallback,
                                doSubsData);
    }

    ~Client() { delete mSs; }

    friend std::ostream &operator<<(std::ostream &os, const Client &obj)
    {
        return os << "Client::" << *(obj.ss()) << ": ";
    }

    bool isValid() const { return (mSs->isValid()); }

    const string &getUserId() const { return mUserId; }

    void setCallId(int callId) { mCallId = callId; }

    int getCallId() const { return mCallId; }

    void setCallSsi(int ssi) { mCallSsi = ssi; }

    int getCallSsi() { return mCallSsi; }

    void setCallingParty(int callingParty) { mCallingParty = callingParty; }

    int getCallingParty() { return mCallingParty; }

    void setGssi(int gssi) { mMonGssi = gssi; }

    int getGssi() const { return mMonGssi; }

    ServerSession *ss() const { return mSs; }

    static void ssCallback(void *obj, MsgSp *msg);

private:
    int             mId;
    int             mCallId;
    int             mCallSsi;
    int             mCallingParty;
    int             mMonGssi;
    string          mUserId;
    ServerSession  *mSs;
    RecvCallbackFn  mCbFn;

    void serverMsg(MsgSp *msg) { ::serverMsg(this, msg); }
};

void Client::ssCallback(void *obj, MsgSp *msg)
{
    if (obj == 0 || msg == 0)
    {
        assert("Bad param in Client::callback" == 0);
        return;
    }
    static_cast<Client *>(obj)->serverMsg(msg);
}

void serverMsg(Client *cl, MsgSp *msg)
{
    LOGGER_DEBUG(gLogger, *cl << "Received " << msg->getName());
    int    val;
    string str;
    string callPartyType;
    bool   nwkMsg = (msg->getMsgId() > 0);
    switch (msg->getType())
    {
        case MsgSp::Type::CALL_PROCEEDING:
        {
            if (nwkMsg)
                cl->setCallId(msg->getFieldInt(MsgSp::Field::CALL_ID));
            break;
        }
        case MsgSp::Type::CALL_RELEASE:
        case MsgSp::Type::SSIC_RELEASE:
        {
            cl->setCallId(0);
            cl->setCallSsi(0);
            break;
        }
        case MsgSp::Type::CALL_SETUP:
        {
            if (nwkMsg)
                cl->setCallId(msg->getFieldInt(MsgSp::Field::CALL_ID));
            val = msg->getFieldInt(MsgSp::Field::CALLING_PARTY);
            cl->setCallSsi(val);
            break;
        }
        case MsgSp::Type::CHANGE_PASSWORD:
        {
            if (msg->isResultSuccessful())
                LOGGER_INFO(gLogger, *cl << msg->getName() << " successful");
            else
                LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed, "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            break;
        }
        case MsgSp::Type::LOGIN:
        {
            LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed, "
                         << msg->getFieldValueString(MsgSp::Field::RESULT));
            break;
        }
        case MsgSp::Type::MON_CONNECT:
        {
            if (msg->getFieldInt(MsgSp::Field::CALL_ID) == cl->getCallId())
            {
                string voipId;
                cl->ss()->ssicInvoke(cl->getCallId(), cl->getGssi(),
                                     cl->getCallingParty(), true, voipId);
            }
            break;
        }
        case MsgSp::Type::MON_INFO:
        {
            if (msg->getFieldInt(MsgSp::Field::CALL_ID) == cl->getCallId())
                cl->setCallId(msg->getFieldInt(MsgSp::Field::NEW_CALL_ID));
            break;
        }
        case MsgSp::Type::MON_SDS:
        {
            if (msg->getFieldInt(MsgSp::Field::SHORT_DATA_TYPE) ==
                MsgSp::Value::SDS_TYPE_4)
            {
                val = msg->getSdsText(str);
                LOGGER_INFO(gLogger, *cl << msg->getName() << ' '
                            << msg->getFieldValueString(
                                               MsgSp::Field::SHORT_DATA_TYPE)
                            << " from "
                            << msg->getFieldInt(MsgSp::Field::CALLING_PARTY)
                            << " to "
                            << msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                            << ": Text-coding " << val << '\n' << str);
            }
            else
            {
                LOGGER_INFO(gLogger, *cl << msg->getName() << ' '
                            << msg->getFieldValueString(
                                               MsgSp::Field::SHORT_DATA_TYPE)
                            << " from "
                            << msg->getFieldInt(MsgSp::Field::CALLING_PARTY)
                            << " to "
                            << msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                            << ": "
                            << msg->getFieldInt(MsgSp::Field::USER_DATA));
            }
            break;
        }
        case MsgSp::Type::MON_SETUP:
        {
            cl->setCallSsi(msg->getFieldInt(MsgSp::Field::CALLING_PARTY));
            if (msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                MsgSp::Value::IDENTITY_TYPE_GSSI)
            {
                cl->setGssi(msg->getFieldInt(MsgSp::Field::CALLED_PARTY));
                cl->setCallingParty(msg->getFieldInt(MsgSp::Field::CALLING_PARTY));
                cl->setCallId(msg->getFieldInt(MsgSp::Field::CALL_ID));
            }
            break;
        }
        case MsgSp::Type::MON_SSDGNA_ASSIGN:
        case MsgSp::Type::MON_SSDGNA_DEASSIGN:
        case MsgSp::Type::MON_SSDGNA_DEFINE:
        case MsgSp::Type::MON_SSDGNA_DELETE:
        case MsgSp::Type::MON_SUBS_ASSIGN:
        case MsgSp::Type::MON_SUBS_DEASSIGN:
        case MsgSp::Type::MON_SUBS_DESC:
        case MsgSp::Type::MON_SUBS_PERMISSION:
        {
            break;
        }
        case MsgSp::Type::MON_START:
        {
            if (msg->isResultSuccessful())
                LOGGER_INFO(gLogger, *cl << msg->getName()
                            << " successful for "
                            << msg->getFieldString(MsgSp::Field::SSI_LIST));
            else
                LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed for "
                             << msg->getFieldString(MsgSp::Field::SSI_LIST)
                             << ": Outgoing "
                             << MsgSp::getFieldName(MsgSp::Field::MSG_ID)
                             << '='
                             << msg->getFieldInt(MsgSp::Field::MSG_ACK)
                             << ", "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            break;
        }
        case MsgSp::Type::MON_STATUS:
        {
            LOGGER_INFO(gLogger, *cl << msg->getName() << " from "
                        << msg->getFieldInt(MsgSp::Field::CALLING_PARTY)
                        << " to "
                        << msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                        << ": "
                        << msg->getFieldInt(MsgSp::Field::STATUS_CODE));
            break;
        }
        case MsgSp::Type::MON_STOP:
        {
            LOGGER_INFO(gLogger, *cl << msg->getName() << " successful for "
                        << msg->getFieldString(MsgSp::Field::SSI_LIST));
            break;
        }
        case MsgSp::Type::MON_SUBS_DEFINE:
        {
            if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                MsgSp::Value::SUBS_TYPE_DISPATCHER)
                LOGGER_INFO(gLogger, *cl << msg->getName() << ": Client "
                            << msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID)
                            << " logged in");
            break;
        }
        case MsgSp::Type::MON_SUBS_DELETE:
        {
            if (msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_TYPE) ==
                MsgSp::Value::SUBS_TYPE_DISPATCHER)
                LOGGER_INFO(gLogger, *cl << msg->getName() << ": Client "
                            << msg->getFieldInt(MsgSp::Field::SUBS_CONTENT_ID)
                            << " logged out");
            break;
        }
        case MsgSp::Type::PASSWORD:
        {
            if (msg->isResultSuccessful())
                LOGGER_INFO(gLogger, *cl << msg->getName()
                            << ": Server login successful");
            else
                LOGGER_ERROR(gLogger, *cl << msg->getName()
                             << ": Server login failed, "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            break;
        }
        case MsgSp::Type::REMOTE_SERVER_DISCONNECTED:
        {
            LOGGER_ERROR(gLogger, *cl << msg->getName());
            break;
        }
        case MsgSp::Type::SDS_RPT:
        {
            switch (msg->getFieldInt(MsgSp::Field::DELIVERY_STATUS))
            {
                case MsgSp::Value::DEL_STAT_MSG_RCVD:
                case MsgSp::Value::DEL_STAT_SDS_RECEIPT_RPT_ACK:
                case MsgSp::Value::DEL_STAT_SDS_CONSUMED:
                case MsgSp::Value::DEL_STAT_SDS_CONSUMED_RPT_ACK:
                case MsgSp::Value::DEL_STAT_SDS_FWD_EXT_NETWORK:
                case MsgSp::Value::DEL_STAT_SENT_TO_GRP:
                case MsgSp::Value::DEL_STAT_CONCATENATION_PART_ACKD:
                case MsgSp::Value::DEL_STAT_CONGESTION_MSG_STORED:
                case MsgSp::Value::DEL_STAT_MSG_STORED:
                case MsgSp::Value::DEL_STAT_DEST_NOT_REACHABLE_MSG_STORED:
                case MsgSp::Value::DEL_STAT_RECEPTION_LOW_LEVEL_ACK:
                    if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                        MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                        LOGGER_INFO(gLogger, *cl << msg->getName()
                                    << ": SDS successful to Dispatcher "
                                    << msg->getFieldInt(
                                               MsgSp::Field::CALLING_PARTY));
                    break;
                default:
                    if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                        MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                        LOGGER_ERROR(gLogger, *cl << msg->getName()
                                     << ": SDS failed to Dispatcher "
                                     << msg->getFieldInt(
                                                 MsgSp::Field::CALLING_PARTY)
                                     << ", "
                                     << msg->getFieldValueString(
                                             MsgSp::Field::DELIVERY_STATUS));
                    else
                        LOGGER_ERROR(gLogger, *cl << msg->getName()
                                     << ": SDS failed, "
                                     << msg->getFieldValueString(
                                             MsgSp::Field::DELIVERY_STATUS));
                    break;
            }
            break;
        }
        case MsgSp::Type::SDS_TRANSFER:
        {
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::UNDEFINED) //received SDS
            {
                if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                    callPartyType = "Dispatcher ";
                if (msg->getFieldInt(MsgSp::Field::SHORT_DATA_TYPE) ==
                    MsgSp::Value::SDS_TYPE_4)
                {
                    val = msg->getSdsText(str);
                    LOGGER_INFO(gLogger, *cl << msg->getName() << ' '
                                << msg->getFieldValueString(
                                               MsgSp::Field::SHORT_DATA_TYPE)
                                << " from " << callPartyType
                                << msg->getFieldInt(
                                                 MsgSp::Field::CALLING_PARTY)
                                << ": Text-coding " << val << '\n' << str);
                }
                else
                {
                    LOGGER_INFO(gLogger, *cl << msg->getName() << ' '
                                << msg->getFieldValueString(
                                               MsgSp::Field::SHORT_DATA_TYPE)
                                << " from " << callPartyType
                                << msg->getFieldInt(
                                                 MsgSp::Field::CALLING_PARTY)
                                << ": "
                                << msg->getFieldInt(MsgSp::Field::USER_DATA));
                }
            }
            else  //error response to sent SDS
            {
                if (msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                    callPartyType = "Dispatcher ";
                LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed to "
                             << callPartyType
                             << msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                             << ": Outgoing "
                             << MsgSp::getFieldName(MsgSp::Field::MSG_ID)
                             << '='
                             << msg->getFieldInt(MsgSp::Field::MSG_ACK)
                             << ", "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            }
            break;
        }
        case MsgSp::Type::STATUS:
        {
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::UNDEFINED) //received status
            {
                if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                    callPartyType = "Dispatcher ";
                LOGGER_INFO(gLogger, *cl << msg->getName() << " from "
                            << callPartyType
                            << msg->getFieldInt(MsgSp::Field::CALLING_PARTY)
                            << ": "
                            << msg->getFieldInt(MsgSp::Field::STATUS_CODE));
            }
            else  //error response to sent status
            {
                if (msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                    callPartyType = "Dispatcher ";
                LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed to "
                             << callPartyType
                             << msg->getFieldInt(MsgSp::Field::CALLED_PARTY)
                             << ": Outgoing "
                             << MsgSp::getFieldName(MsgSp::Field::MSG_ID)
                             << '='
                             << msg->getFieldInt(MsgSp::Field::MSG_ACK)
                             << ", "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            }
            break;
        }
        case MsgSp::Type::STATUS_DATA:
        {
            LOGGER_INFO(gLogger, *cl << "Received\n" << *msg);
            break;
        }
        case MsgSp::Type::STATUS_RPT:
        {
            if (!msg->isResultSuccessful())
            {
                if (msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE) ==
                    MsgSp::Value::IDENTITY_TYPE_DISPATCHER)
                    LOGGER_ERROR(gLogger, *cl << msg->getName()
                                 << ": Status Message failed to Dispatcher "
                                 << msg->getFieldInt(
                                             MsgSp::Field::CALLING_PARTY)
                                 << ", "
                                 << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
                else
                    LOGGER_ERROR(gLogger, *cl << msg->getName()
                                 << ": Status Message failed, "
                                 << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            }
            break;
        }
        case MsgSp::Type::SUBS_DATA:
        {
            if (msg->isResultSuccessful())
                LOGGER_INFO(gLogger, *cl << msg->getName() << " successful");
            else
                LOGGER_ERROR(gLogger, *cl << msg->getName() << " failed, "
                             << msg->getFieldValueString(
                                                      MsgSp::Field::RESULT));
            break;
        }
        case MsgSp::Type::SYS_ERROR:
        {
            LOGGER_ERROR(gLogger, *cl << msg->getName()
                         << ": Failed to send "
                         << MsgSp::getTypeName(msg->getFieldInt(
                                             MsgSp::Field::INVALID_MSG_TYPE))
                         << ' ' << MsgSp::getFieldName(MsgSp::Field::MSG_ID)
                         << '=' << msg->getFieldInt(MsgSp::Field::MSG_ACK)
                         << " because of "
                         << MsgSp::getFieldName(MsgSp::Field::INVALID_FIELD)
                         << ' '
                         << msg->getFieldValueString(msg->getFieldInt(
                                              MsgSp::Field::INVALID_FIELD)));
            break;
        }
        default:
        {
            LOGGER_WARNING(gLogger, *cl << msg->getName() << " unhandled");
            break;
        }
    } //switch (msg->getType())
    delete msg;
}

void sigHandler(int sig)
{
    if (gLogger != 0)
        LOGGER_INFO(gLogger, "Signal " << sig << ". gSignallable="
                    << gSignallable << '.');
    if (gSignallable && sem_post(&gSignalSem) != 0)
    {
        if (gLogger != 0)
            LOGGER_ERROR(gLogger, "Failed to terminate application. "
                         "sem_post() failed with errno " << errno
                         << Socket::getErrorStr(errno)
                         << ". Retrying once.");
        sem_post(&gSignalSem);
    }
    close(0); //close stdin to interrupt getline() in CLI mode
}

void testDgnaAssign(Client *cl,
                    int     gssi,
                    int     ssi,
                    bool    isIndiv,
                    bool    doAssign)
{
    cl->ss()->ssDgnaAssign(gssi, ssi, isIndiv, doAssign);
}

void testSend(Client *cl, int msgType)
{
    switch (msgType)
    {
        case MsgSp::Type::SDS_TRANSFER:
        {
            cl->ss()->sds(MsgSp::Value::IDENTITY_TYPE_ISSI, 3204684,
                          "Test SDS");
            break;
        }
        case MsgSp::Type::SSDGNA_ASSIGN:
        {
            testDgnaAssign(cl, 3204992, 3204681, true, true);
            break;
        }
        case MsgSp::Type::SSDGNA_DEASSIGN:
        {
            testDgnaAssign(cl, 3204992, 3204681, true, false);
            break;
        }
        default:
        {
            MsgSp *m = new MsgSp(msgType);
            cl->ss()->sendMsg(m);
            break;
        }
    }
}

void usage(const string &myName)
{
    cout << "\nUsage: " << myName << " [-hpqst]\n\n"
            "  h:      Show this message and exit.\n"
            "  p port: Main server port number.\n"
            "  q port: Redundant server port number.\n"
            "  s IP:   Main server IP.\n"
            "  t IP:   Redundant server IP."
         << endl;
}

string getClientIds(const ClientsMapT &cMap)
{
    ostringstream oss;
    oss << "\nClients:";
    if (cMap.empty())
    {
        oss << " None";
    }
    else
    {
        ClientsMapT::const_iterator it = cMap.begin();
        for (; it!=cMap.end(); ++it)
        {
            oss << ' ' << it->first;
        }
    }
    return oss.str();
}

int main(int argc, char *argv[])
{
    int    serverPort1 = 5055;
    int    serverPort2 = 5056;
    string serverIp1(Socket::LOCALHOST);
    string serverIp2(Socket::LOCALHOST);

    //process command line options
    int c;
    while ((c = getopt(argc, argv, "hp:q:s:t:")) != EOF)
    {
        switch (c)
        {
            case 'h':
                usage(argv[0]);
                return 0;
            case 'p':
                serverPort1 = Utils::fromString<int>(string(optarg));
                break;
            case 'q':
                serverPort2 = Utils::fromString<int>(string(optarg));
                break;
            case 's':
                serverIp1 = string(optarg);
                break;
            case 't':
                serverIp2 = string(optarg);
                break;
            default:
                break; //do nothing
        }
    }

    gLogger = new Logger();
    LOGGER_RAW(gLogger, LOG_HEADER);
    LOGGER_INFO(gLogger, "PID = " << getpid());
    LOGGER_INFO(gLogger, "Servers " << serverIp1 << ':' << serverPort1
                << ' ' << serverIp2 << ':'  << serverPort2);

    int           intParam;
    int           cid;       //client ID
    int           callId;
    int           ssi;
    unsigned int  uintParam;
    bool          boolParam;
    bool          encryption = true;
    string        input;
    string        lastInput;
    string        cmd;
    string        strParam;
    string        fnName;
    Client       *client;

    ServerSession::init(gLogger, serverIp1, serverPort1, serverIp2,
                        serverPort2);
    ServerSession::setVersion(CLIENT_VERSION);
    ClientsMapT clientsMap;

    for (;;)
    {
        cerr << "SCADC> ";
        getline(cin, input);
        if (input == "q")
            break;
        if (input.empty())
            continue;

        if (input == "r")
            input = lastInput;
        else
            lastInput = input;

        istringstream is(input);
        is >> cmd;
        if (!is)
            continue;

        intParam  = -1;
        cid       = -1;
        callId    = 0;
        ssi       = 0;
        uintParam = 0;
        client    = 0;
        strParam.clear();
        //configuration commands ============================================
        if (cmd == "encr")
        {
            cout << "Encryption ";
            if (is >> intParam)
            {
                encryption = (intParam != 0);
                ClientsMapT::iterator it = clientsMap.begin();
                for (; it!=clientsMap.end(); ++it)
                {
                    it->second->ss()->setEncryption(encryption);
                }
            }
            cout << ((encryption)? "on": "off") << endl;
            if (intParam < 0)
                cout << "To set: " << cmd << " <0(off)|1(on)>" << endl;
        }

        else if (cmd == "ll")
        {
            cout << "LogLevel ";
            if ((is >> strParam) && gLogger->setLevel(strParam))
            {
                cout << gLogger->getLevel() << endl;
            }
            else
            {
                if (is)
                    cout << strParam << " failed" << endl;
                else
                    cout << gLogger->getLevel() << endl;
                cout << "To set, specify the level\n  " << cmd << ' '
                     << gLogger->getValidLevels() << endl;
            }
        }

        else if (cmd == "sf")
        {
            cout << "Fleet ";
            is >> intParam;
            if (is && (intParam >= 0))
                gFleetId = intParam;
            cout << gFleetId << endl;
            if (!is)
                cout << "To set: " << cmd << " <fleetId>" << endl;
        }

        //actions commands ==================================================
        else if (cmd == "conn")
        {
            fnName = "Connect";
            if (is >> cid)
            {
                do
                {
                    if (clientsMap.count(cid) == 0)
                    {
                        cout << fnName << " Client-" << cid << endl;
                        client = new Client(cid, &serverMsg);
                        if (client->isValid())
                            clientsMap[cid] = client;
                        else
                            delete client;
                    }
                    else
                    {
                        cout << fnName << ": Client-" << cid
                             << " already started: " << *(clientsMap[cid])
                             << endl;
                    }
                }
                while (is >> cid);
            }
            else if (clientsMap.empty())
            {
                cid = 1001;
                cout << fnName << " Client-" << cid << endl;
                client = new Client(cid, &serverMsg);
                if (client->isValid())
                    clientsMap[cid] = client;
                else
                    delete client;
            }
            else
            {
                cout << fnName << "\nUsage: " << cmd
                     << " <cid1> [<cid2> ...]" << endl;
            }
        }

        else if (cmd == "disc")
        {
            //cmd <cid1> <cid2> ...
            fnName = "Disconnect";
            if (is >> cid)
            {
                do
                {
                    if (clientsMap.count(cid) != 0)
                    {
                        cout << fnName << " Client-" << cid << endl;
                        delete clientsMap[cid];
                        clientsMap.erase(cid);
                    }
                }
                while (is >> cid);
            }
            else if (!clientsMap.empty())
            {
                cid = clientsMap.begin()->first;
                cout << fnName << " Client-" << cid << endl;
                delete clientsMap[cid];
                clientsMap.erase(cid);
            }
            else
            {
                cout << fnName << "\nUsage: " << cmd
                     << " [<cid1> <cid2> ...]"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "send")
        {
            cout << "SendMessage";
            if ((is >> cid >> intParam) && clientsMap.count(cid) != 0 &&
                intParam >= 0)
            {
                cout << " Client-" << cid << ' ' << intParam << endl;
                testSend(clientsMap[cid], intParam);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <msgType>"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "rsend")
        {
            //rsend [<maxType>]
            intParam = 99;
            is >> intParam;
            if (!clientsMap.empty())
            {
                int i;
                client = clientsMap.begin()->second;
                for (i=1; i<=intParam; ++i)
                {
                    testSend(client, i);
                }
            }
            else
            {
                cout << "No client" << endl;
            }
        }

        else if (cmd == "rst")
        {
            cout << "requestStatusData";
            if (is >> cid)
            {
                if (clientsMap.count(cid) != 0)
                    client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid = clientsMap.begin()->first;
                client = clientsMap.begin()->second;
            }
            if (client != 0)
            {
                cout << " Client-" << cid << endl;
                client->ss()->requestStatusData();
            }
            else
            {
                cout << "\nUsage: " << cmd << " [<cid>]"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "rsd")
        {
            cout << "requestSubsData";
            if (is >> cid)
            {
                if (clientsMap.count(cid) != 0)
                    client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid = clientsMap.begin()->first;
                client = clientsMap.begin()->second;
            }
            if (client != 0)
            {
                cout << " Client-" << cid << endl;
                client->ss()->requestSubsData();
            }
            else
            {
                cout << "\nUsage: " << cmd << " [<cid>]"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "mon")
        {
            boolParam = false;
            if (is >> intParam)
            {
                cout << "monitor" << ((intParam != 0)? "Start": "Stop");
                if ((is >> cid) && clientsMap.count(cid) != 0)
                {
                    bool isGroup;
                    if (is >> isGroup)
                    {
                        getline(is, strParam);
                        set<int> vals;
                        if (Utils::fromStringWithRange(strParam, vals) > 0)
                        {
                            boolParam = true;
                            cout << " Client-" << cid << ' '
                                 << "Type-" << ((isGroup)? "GSSI ": "SSI ")
                                 << Utils::toString(vals, ' ') << endl;
                            if (intParam != 0)
                                clientsMap[cid]->ss()->monitorStart(vals,
                                                                    isGroup);
                            else
                                clientsMap[cid]->ss()->monitorStop(vals,
                                                                   isGroup);
                        }
                    }
                }
                if (!boolParam)
                    cmd.append((intParam != 0)? " 1": " 0");
            }
            else
            {
                cout << "monitorStart/Stop";
                cmd.append(" <1|0>");
            }
            if (!boolParam)
                cout << "\nUsage: " << cmd << " <cid> <0-ISSI|1-GSSI> "
                     << "<SSI list>" << getClientIds(clientsMap) << endl;
        }

        else if (cmd == "sds" || cmd == "sdsg" || cmd == "sdsd")
        {
            cout << MsgSp::getTypeName(MsgSp::Type::SDS_TRANSFER);
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                getline(is, strParam);
                if (Utils::trim(strParam) > 0)
                {
                    cout << " Client-" << cid << ' ';
                    if (cmd == "sds")
                    {
                        cout << ssi << endl;
                        clientsMap[cid]->ss()->sds(
                                            MsgSp::Value::IDENTITY_TYPE_ISSI,
                                            ssi, strParam);

                    }
                    else if (cmd == "sdsg")
                    {
                        cout << "GSSI-" << ssi << endl;
                        clientsMap[cid]->ss()->sds(
                                            MsgSp::Value::IDENTITY_TYPE_GSSI,
                                            ssi, strParam);
                    }
                    else
                    {
                        cout << "Dispatcher-" << ssi << endl;
                        clientsMap[cid]->ss()->sds(
                                      MsgSp::Value::IDENTITY_TYPE_DISPATCHER,
                                      ssi, strParam);
                    }
                }
            }
            if (strParam.empty())
                cout << "\nUsage: " << cmd << " <cid> <ssi> <message-string>"
                     << getClientIds(clientsMap) << endl;
        }

        else if (cmd == "status" || cmd == "statusg" || cmd == "statusd")
        {
            cout << MsgSp::getTypeName(MsgSp::Type::STATUS);
            if ((is >> cid >> ssi >> intParam) &&
                (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << ' ';
                if (cmd == "status")
                {
                    cout << ssi << ' ' << intParam << endl;
                    clientsMap[cid]->ss()->status(
                                            MsgSp::Value::IDENTITY_TYPE_ISSI,
                                            ssi, intParam);
                }
                else if (cmd == "statusg")
                {
                    cout << "GSSI-" << ssi << ' ' << intParam << endl;
                    clientsMap[cid]->ss()->status(
                                            MsgSp::Value::IDENTITY_TYPE_GSSI,
                                            ssi, intParam);
                }
                else
                {
                    cout << "Dispatcher-" << ssi << ' ' << intParam << endl;
                    clientsMap[cid]->ss()->status(
                                      MsgSp::Value::IDENTITY_TYPE_DISPATCHER,
                                      ssi, intParam);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <ssi> <status-value>"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "csg")
        {
            cout << "callSetupGrp";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                client->setCallSsi(ssi);
                if (is >> intParam) //priority
                {
                    cout << " Client-" << cid << ' ' << ssi << ' '
                         << intParam << endl;
                    client->ss()->callSetupGrp(ssi, strParam, intParam);
                }
                else
                {
                    cout << " Client-" << cid << ' ' << ssi << endl;
                    client->ss()->callSetupGrp(ssi, strParam);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <gssi> [<priority>]"
                     << endl;
            }
        }

        else if (cmd == "csi" || cmd == "csip")
        {
            boolParam = (cmd == "csi");
            cout << "callSetupInd ";
            if (boolParam)
                cout << "duplex";
            else
                cout << "PTT";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                client->setCallSsi(ssi);
                if (is >> intParam) //priority
                {
                    cout << " Client-" << cid << ' ' << ssi << ' '
                         << intParam << endl;
                    client->ss()->callSetupInd(ssi, boolParam, true,
                                               strParam, intParam);
                }
                else
                {
                    cout << " Client-" << cid << ' ' << ssi << endl;
                    client->ss()->callSetupInd(ssi, boolParam, true,
                                               strParam);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <issi> [<priority>]"
                     << endl;
            }
        }

        else if (cmd == "csd")
        {
            cout << "callSetupInd dispatcher";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                cout << " Client-" << cid << ' ' << ssi << endl;
                client->ss()->callSetupInd(ssi, strParam);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <cid2>" << endl;
            }
        }

        else if (cmd == "csa")
        {
            cout << "callSetupAmbience";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                client->setCallSsi(ssi);
                if (is >> intParam) //priority
                {
                    cout << " Client-" << cid << ' ' << ssi << ' '
                         << intParam << endl;
                    client->ss()->callSetupAmbience(ssi, strParam, intParam);
                }
                else
                {
                    cout << " Client-" << cid << ' ' << ssi << endl;
                    client->ss()->callSetupAmbience(ssi, strParam);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <issi> [<priority>]"
                     << endl;
            }
        }

        else if (cmd == "csb")
        {
            cout << "callSetupBroadcast";
            if ((is >> cid) && (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << endl;
                client = clientsMap[cid];
                client->setCallSsi(ServerSession::BROADCAST_SSI);
                client->ss()->callSetupBroadcast(strParam);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid>" << endl;
            }
        }

        else if (cmd == "cc")
        {
            cout << "callConnect";
            if ((is >> cid) && clientsMap.count(cid) != 0)
            {
                client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid    = clientsMap.begin()->first; //single client testing
                client = clientsMap[cid];
            }
            if (client != 0)
            {
                callId = client->getCallId();
                ssi    = client->getCallSsi();
            }
            if (client != 0 && callId != 0 && ssi != 0)
            {
                cout << " Client-" << cid << ' ' << callId << ' ' << ssi
                     << endl;
                client->ss()->callConnect(callId, ssi);
            }
            else
            {
                cout << "\nUsage: " << cmd << " [<cid>]"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "ccd")
        {
            cout << "callConnect dispatcher";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                cout << " Client-" << cid << ' ' << ssi << endl;
                client->ss()->callConnect(ssi);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <cid2>" << endl;
            }
        }

        else if (cmd == "cd")
        {
            cout << "callDisconnect";
            if ((is >> cid) && clientsMap.count(cid) != 0)
            {
                client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid    = clientsMap.begin()->first; //single client testing
                client = clientsMap[cid];
            }
            if (client != 0)
            {
                callId = client->getCallId();
                ssi    = client->getCallSsi();
            }
            if (client != 0 && callId != 0 && ssi != 0)
            {
                cout << " Client-" << cid << ' ' << callId << ' ' << ssi
                     << endl;
                client->ss()->callDisconnect(callId, ssi);
            }
            else
            {
                cout << "\nUsage: " << cmd << " [<cid>]"
                     << getClientIds(clientsMap) << endl;
            }
        }

        else if (cmd == "cdd")
        {
            cout << "callDisconnect dispatcher";
            if ((is >> cid >> ssi) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                cout << " Client-" << cid << ' ' << ssi << endl;
                client->ss()->callDisconnect(ssi);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <cid2>" << endl;
            }
        }

        else if (cmd == "d")
        {
            cout << "callTxDemand";
            if ((is >> cid) && clientsMap.count(cid) != 0)
            {
                client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid    = clientsMap.begin()->first; //single client testing
                client = clientsMap[cid];
            }
            if (client != 0)
                callId = client->getCallId();
            is >> intParam; //last param is priority
            if (client == 0 || callId == 0)
            {
                cout << "\nUsage: " << cmd << " [<cid> [<priority>]]"
                     << getClientIds(clientsMap) << endl;
            }
            else if (intParam >= 0)
            {
                cout << " Client-" << cid << ' ' << callId << ' '
                     << intParam << endl;
                client->ss()->callTxDemand(callId, intParam);
            }
            else
            {
                cout << " Client-" << cid << ' ' << callId << endl;
                client->ss()->callTxDemand(callId);
            }
        }

        else if (cmd == "c")
        {
            cout << "callTxCeased";
            if ((is >> cid) && clientsMap.count(cid) != 0)
            {
                client = clientsMap[cid];
            }
            else if (!clientsMap.empty())
            {
                cid    = clientsMap.begin()->first; //single client testing
                client = clientsMap[cid];
            }
            if (client != 0)
                callId = client->getCallId();
            if (client == 0 || callId == 0)
            {
                cout << "\nUsage: " << cmd << " [<cid>]"
                     << getClientIds(clientsMap) << endl;
            }
            else
            {
                cout << " Client-" << cid << ' ' << callId << endl;
                client->ss()->callTxCeased(callId);
            }
        }

        else if (cmd == "ct")
        {
            cout << "callTerminate";
            if ((is >> cid >> callId) && (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << ' ' << callId << endl;
                clientsMap[cid]->ss()->callTerminate(callId);
            }
            else
            {
                cout << "\nUsage: " << cmd << " <cid> <call-ID>" << endl;
            }
        }

        else if (cmd == "cii")
        {
            cout << "ssicInvoke ";
            if ((is >> cid >> callId) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                cout << " Client-" << cid << ' ' << callId << endl;
                client->setCallId(callId);
                client->ss()->ssicInvoke(callId, client->getGssi(),
                                         client->getCallSsi(), false,
                                         strParam);
            }
            else
            {
                cout << "Usage: " << cmd << " <cid> <call-ID>" << endl;
            }
        }

        else if (cmd == "cid")
        {
            cout << "ssicDisconnect ";
            if ((is >> cid >> callId) && (clientsMap.count(cid) != 0))
            {
                client = clientsMap[cid];
                cout << " Client-" << cid << ' ' << callId << endl;
                client->ss()->ssicDisconnect(callId, client->getGssi());
            }
            else
            {
                cout << "Usage: " << cmd << " <cid> <call-ID>" << endl;
            }
        }

        else if (cmd == "clc")
        {
            cout << "listenConnect ";
            if ((is >> cid >> callId >> ssi) && (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << ' ' << callId << ' ' << ssi
                     << endl;
                clientsMap[cid]->ss()->listenConnect(callId, ssi, strParam);
            }
            else
            {
                cout << "Usage: " << cmd << " <cid> <call-ID> <ISSI>"
                     << endl;
            }
        }

        else if (cmd == "cld")
        {
            cout << "listenDisconnect ";
            if ((is >> cid >> callId >> ssi) && (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << ' ' << callId << ' ' << ssi
                     << endl;
                clientsMap[cid]->ss()->listenDisconnect(callId, ssi);
            }
            else
            {
                cout << "Usage: " << cmd << " <cid> <call-ID> <ISSI>"
                     << endl;
            }
        }

        else if (cmd == "pc")
        {
            cout << "changePasswd";
            string newPasswd;
            if ((is >> cid >> strParam >> newPasswd) &&
                (clientsMap.count(cid) != 0))
            {
                cout << " Client-" << cid << ' ' << strParam << ' '
                     << newPasswd << endl;
                clientsMap[cid]->ss()->changePasswd(strParam, newPasswd);
            }
            else
            {
                cout << "\nUsage: " << cmd
                     << " <cid> <old-password> <new-password>"
                     << getClientIds(clientsMap) << endl;
            }
        }

        //query commands ====================================================
        else if (cmd == "gst")
        {
            cout << "StatusCodes::getData\n" << StatusCodes::getData()
                 << endl;
        }

        else if (cmd == "gstt")
        {
            is >> intParam;
            cout << "StatusCodes::getTimestampStr\n"
                 << StatusCodes::getTimestampStr((intParam > 0)) << endl;
        }

        else if (cmd == "ggn")
        {
            cout << "SubsData::getGrpName";
            if (is >> ssi)
                wcout << "(" << ssi << ") = "
                      << Utils::hex2Wstring(SubsData::getGrpName(ssi))
                      << endl;
            else
                cout << "\nUsage: " << cmd << " <gssi>" << endl;
        }

        else if (cmd == "ggs")
        {
            cout << "SubsData::getGroups";
            SubsData::Ssi2DescMapT stGrps;
            SubsData::Ssi2DescMapT diGrps;
            SubsData::Ssi2DescMapT dgGrps;
            SubsData::Ssi2DescMapT::iterator it;
            SubsData::getGroups(stGrps, diGrps, dgGrps);
            cout << "\nStatic: " << stGrps.size() << " :::::::::::\n";
            for (it=stGrps.begin(); it!=stGrps.end(); ++it)
            {
                if (!it->second.empty())
                    wcout << Utils::hex2Wstring(it->second) << ": ";
                cout << it->first << ", ";
            }
            cout << "\nDGNA-Ind: " << diGrps.size() << " :::::::::::\n";
            for (it=diGrps.begin(); it!=diGrps.end(); ++it)
            {
                if (!it->second.empty())
                    wcout << Utils::hex2Wstring(it->second) << ": ";
                cout << it->first << ", ";
            }
            cout << "\nDGNA-Grp: " << dgGrps.size() << " :::::::::::\n";
            for (it=dgGrps.begin(); it!=dgGrps.end(); ++it)
            {
                if (!it->second.empty())
                    wcout << Utils::hex2Wstring(it->second) << ": ";
                cout << it->first << ", ";
            }
            cout << endl;
        }

        else if (cmd == "ggm")
        {
            cout << "SubsData::getGrpMembers";
            if (is >> ssi)
            {
                wcout << "(" << ssi << ") "
                      << Utils::hex2Wstring(SubsData::getGrpName(ssi))
                      << "\n";
                cout << SubsData::getGrpMembers(ssi) << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <gssi>" << endl;
            }
        }

        else if (cmd == "ggam")
        {
            cout << "SubsData::getGrpAttachedMembers";
            if (is >> ssi)
            {
                wcout << "(" << ssi << ") "
                      << Utils::hex2Wstring(SubsData::getGrpName(ssi))
                      << "\n";
                cout << SubsData::getGrpAttachedMembers(ssi, true) << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <gssi>" << endl;
            }
        }

        else if (cmd == "gi")
        {
            //cmd <fleet>
            cout << "SubsData::getIssis ";
            if (is >> intParam)
                cout << intParam << " = " << SubsData::getIssis(intParam)
                     << endl;
            else
                cout << "= " << SubsData::getIssis() << endl;
        }

        else if (cmd == "gsdc")
        {
            cout << "SubsData::getCounts\n" << SubsData::getCounts() << endl;
        }

        else if (cmd == "gsd")
        {
            cout << "SubsData::getData\n" << SubsData::getData() << endl;
        }

        else if (cmd == "gsdt")
        {
            is >> intParam;
            cout << "SubsData::getTimestampStr\n"
                 << SubsData::getTimestampStr((intParam > 0)) << endl;
        }

        else if (cmd == "gsdd")
        {
            cout << "SubsData::getClientData\n"
                 << SubsData::getClientData(true) << endl;
        }

        else if (cmd == "gsdg")
        {
            cout << "SubsData::getDgnaAllowedGssis";
            if (is >> ssi)
            {
                wcout << "(" << ssi << ") "
                      << Utils::hex2Wstring(SubsData::getGrpName(ssi))
                      << ": ";
                SubsData::IdSetT ssis;
                SubsData::getDgnaAllowedGssis(ssi, ssis);
                cout << ssis.size() << "\n"
                     << Utils::toString(ssis, ',') << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <gssi>" << endl;
            }
        }

        else if (cmd == "gsdi")
        {
            cout << "SubsData::getDgnaAllowedIssis";
            if (is >> ssi)
            {
                wcout << "(" << ssi << ") "
                      << Utils::hex2Wstring(SubsData::getGrpName(ssi))
                      << ": ";
                SubsData::IdSetT ssis;
                SubsData::getDgnaAllowedIssis(ssi, ssis);
                cout << ssis.size() << "\n"
                     << Utils::toString(ssis, ',') << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <gssi>" << endl;
            }
        }

        //load test commands ================================================
        else if (cmd == "ltc")
        {
            cout << "Load Test Connect";
            boolParam = false;
            if (is >> strParam)
            {
                for (auto &it : clientsMap)
                {
                    delete it.second;
                }
                clientsMap.clear();
                set<int> vals;
                if (Utils::fromStringWithRange(strParam, vals) > 0 &&
                    vals.size() <= MAXCLIENTS)
                {
                    for (auto id : vals)
                    {
                        client = new Client(id, &serverMsg,
                                            (id == *vals.begin()));
                        if (client->isValid())
                            clientsMap[id] = client;
                        else
                            delete client;
                        //wait for some time to prevent server disconnection due
                        //to SYN flood attack protection
                        sleep(3);
                    }
                    boolParam = true;
                }
            }
            if (!boolParam)
            {
                cout << "\nUsage: " << cmd << " <id1-id2>"
                     << "\nMaximum number of clients: " << MAXCLIENTS << endl;
            }
        }
        else if (cmd == "ltd") //disconnect
        {
            cout << "Load Test Disconnect";
            intParam = clientsMap.size();
            for (auto &it : clientsMap)
            {
                delete it.second;
            }
            clientsMap.clear();
            cout << "\nDestroyed " << intParam << " clients" << endl;
        }
        else if (cmd == "ltmg")
        {
            cout << "Load Test Monitor Group";
            if ((is >> intParam >> ssi) && ssi > 0 && intParam >= 0)
            {
                for (auto &it : clientsMap)
                {
                    if (intParam != 0)
                        it.second->ss()->monitorStart(ssi, true);
                    else
                        it.second->ss()->monitorStop(ssi, true);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <0:stop|1:start> <gssi>" << endl;
            }
        }
        else if (cmd == "ltcsi")
        {
            cout << "Load Test callSetupInd duplex";
            if (is >> ssi)
            {
                for (auto &it : clientsMap)
                {
                    it.second->ss()->callSetupInd(ssi, true, true, strParam);
                    it.second->setCallSsi(ssi);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <issi>" << endl;
            }
        }
        else if (cmd == "ltcd")
        {
            cout << "Load Test callDisconnect" << endl;
            for (auto &it : clientsMap)
            {
                it.second->ss()->callDisconnect(it.second->getCallId(),
                                                it.second->getCallSsi());
            }
        }
        else if (cmd == "ltgps")
        {
            cout << "Load Test GPS";
            if ((is >> ssi >> intParam) && ssi > 0 && intParam > 0)
            {
                for (; intParam>0; --intParam)
                {
                    for (auto &it : clientsMap)
                    {
                        it.second->ss()->sdsGps(ssi,
                                             GPSDATA[intParam % GPSDATA_COUNT]);
                    }
                    sleep(1);
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <issi> <count>" << endl;
            }
        }
        else if (cmd == "ltsds")
        {
            cout << "Load Test SDS";
            if ((is >> ssi >> intParam) && ssi > 0 && intParam > 0)
            {
                input.assign("From TestClient: Message ");
                int i = 1;
                for (; i<=intParam; ++i)
                {
                    for (auto &it : clientsMap)
                    {
                        it.second->ss()->sds(MsgSp::Value::IDENTITY_TYPE_ISSI,
                                             ssi, input + std::to_string(i));
                        usleep(10000);
                    }
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <issi> <count>" << endl;
            }
        }
        else if (cmd == "ltsm")
        {
            cout << "Load Test Status Message";
            if ((is >> ssi >> intParam) && ssi > 0 && intParam > 0)
            {
                int i = 1;
                for (; i<=intParam; ++i)
                {
                    for (auto &it : clientsMap)
                    {
                        it.second->ss()->status(MsgSp::Value::IDENTITY_TYPE_ISSI,
                                                ssi, i);
                        usleep(10000);
                    }
                }
            }
            else
            {
                cout << "\nUsage: " << cmd << " <issi> <count>" << endl;
            }
        }

        //internal function tests ===========================================
        else if (cmd == "ufs")
        {
            cout << "Utils::fromString<int> ";
            if (is >> strParam)
                cout << strParam << " = " << Utils::fromString<int>(strParam)
                     << endl;
            else
                cout << "\nUsage: " << cmd << " <string>" << endl;
        }

        else if (cmd == "ufss")
        {
            cout << "Utils::fromString ";
            getline(is, strParam);
            if (Utils::trim(strParam) > 0)
            {
                set<int> vals;
                intParam = Utils::fromStringWithRange(strParam, vals);
                cout << '"' << strParam << "\" = [" << intParam << "]: "
                     << Utils::toString(vals, ' ') << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <list of integers>" << endl;
            }
        }

        else if (cmd == "ufh")
        {
            cout << "Utils::fromHexString ";
            if (is >> strParam)
                cout << strParam << " = " << Utils::fromHexString(strParam)
                     << endl;
            else
                cout << "\nUsage: " << cmd << " <hexString>" << endl;
        }

        else if (cmd == "uth")
        {
            cout << "Utils::toHexString ";
            if (is >> uintParam)
            {
                if (is >> intParam)
                    cout << uintParam << ' ' << intParam << " = "
                         << Utils::toHexString(uintParam, intParam) << endl;
                else
                    cout << uintParam << " = "
                         << Utils::toHexString(uintParam) << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <unsigned int> [<len>]"
                     << endl;
            }
        }

        else if (cmd == "mhs")
        {
            cout << "MsgSp::hexScramble ";
            string key;
            if (is >> strParam)
            {
                is >> key;
                string s(MsgSp::hexScramble(strParam, key));
                cout << strParam << ' ' << key << " = " << s
                     << "\nMsgSp::hexUnscramble " << s << ' ' << key << " = "
                     << MsgSp::hexUnscramble(s, key) << endl;
            }
            else
            {
                cout << "\nUsage: " << cmd << " <string> <key>" << endl;
            }
        }
    } //for (;;)

    while (!clientsMap.empty())
    {
        delete (clientsMap.begin())->second;
        clientsMap.erase(clientsMap.begin());
    }

    delete gLogger;
    PalSocket::finalize();
    return 0;
}
