/**
 * VOIP session module for a client application.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: VoipSessionClient.h 1802 2024-01-05 06:41:50Z zulzaidi $
 * @author Ahmad Syukri
 */
#ifndef VOIPSESSIONCLIENT_H
#define VOIPSESSIONCLIENT_H

#include <string>

#include "Logger.h"
#include "MsgSip.h"
#include "MsgSp.h"
#include "VoipSessionBase.h"

class VoipSessionClient : public VoipSessionBase
{
public:
    /**
     * Constructor.
     *
     * @param[in] userId      User ID for registration.
     * @param[in] version     Client version.
     * @param[in] localIp     Local IP address.
     * @param[in] serverIp    VOIP Server IP address.
     * @param[in] strCbFn     The callback function to pass a log string.
     * @param[in] logger      A logger object. Caller retains ownership.
     * @param[in] callbackObj Owner of the callback function.
     * @param[in] callbackFn  The callback function to pass a received message.
     */
    VoipSessionClient(int                userId,
                      const std::string &version,
                      const std::string &localIp,
                      const std::string &serverIp,
#ifdef MOBILE
                      StrCallbackFn      strCbFn,
#else
                      Logger            *logger,
                      void              *callbackObj,
#endif
                      RecvCallbackFn     callbackFn);

    /**
     * Sets the local IP and calls VoipSessionBase::setSipFields().
     *
     * @param[in] ip The local IP.
     */
    void setLocalIp(const std::string &ip);

    /**
     * Switches and registers to a new server.
     *
     * @param[in] ip   The server IP.
     * @param[in] port The server port, if different from the current or
     *                 default.
     * @return true if successful.
     */
    bool setServer(const std::string &ip, int port = 0);

    /**
     * Re-registers to server if it is unchanged. Otherwise calls setServer().
     * Also sets the local IP if changed.
     *
     * @param[in] localIp The local IP.
     * @param[in] svrIp   The server IP.
     */
    void reregister(const std::string &localIp = "",
                    const std::string &svrIp = "");

    /**
     * Makes an individual duplex call.
     *
     * @param[in] calledParty Called number.
     * @param[in] params      Call parameters, if any.
     * @return The call ID.
     */
    std::string callSetupIndDuplex(int                calledParty,
                                   const std::string &params = "");

    /**
     * Makes an individual client call.
     *
     * @param[in] calledParty  Called number.
     * @param[in] calledDomain The VOIP called party domain.
     * @param[in] params       Call parameters, if any.
     * @return The call ID.
     */
    std::string callSetupIndDisp(int                calledParty,
                                 const std::string &calledDomain,
                                 const std::string &params = "");

    /**
     * Makes an individual client video call.
     *
     * @param[in] calledParty  Called number.
     * @param[in] calledDomain The VOIP called party domain.
     * @param[in] params       Call parameters, if any.
     * @return The call ID.
     */
    std::string callSetupVideo(int                calledParty,
                               const std::string &calledDomain,
                               const std::string &params = "");

#ifdef MOBILE
    /**
     * Makes a dispatcher call.
     *
     * @param[in] calledParty Called number.
     * @param[in] params      Call parameters.
     * @return The call ID.
     */
    std::string callSetupDisp(int calledParty, const std::string &params = "");

    /**
     * Makes a dispatcher video call.
     *
     * @param[in] calledParty Called number.
     * @param[in] params      Call parameters.
     * @return The call ID.
     */
    std::string callSetupVideoDisp(int                calledParty,
                                   const std::string &params = "");
#endif

    /**
     * Makes an individual PTT call.
     *
     * @param[in] calledParty Called number.
     * @param[in] params      Call parameters.
     * @return The call ID.
     */
    std::string callSetupIndPtt(int                calledParty,
                                const std::string &params = "");

    /**
     * Makes a group call.
     *
     * @param[in] calledParty Called number.
     * @param[in] priority    Call priority.
     * @return The call ID.
     */
    std::string callSetupGrp(int calledParty, int priority);

    /**
     * Makes a broadcast call.
     *
     * @param[in] calledParty Called number.
     * @return The call ID.
     */
    std::string callSetupBroadcast(int calledParty);

    /**
     * Joins a group call.
     *
     * @param[in] calledParty  Called number.
     * @param[in] callingParty Calling number.
     * @param[in] isDisp       true for call started by dispatcher.
     * @return The call ID.
     */
    std::string callSetupSsic(int calledParty, int callingParty, bool isDisp);

    /**
     * Makes an ambience call.
     *
     * @param[in] calledParty Called number.
     * @param[in] params      Call parameters.
     * @return The call ID.
     */
    std::string callSetupAmbience(int                calledParty,
                                  const std::string &params = "");

    /**
     * Makes a listening call to a calling party in an individual call.
     *
     * @param[in] calledParty The calling party to listen to.
     * @return The call ID.
     */
    std::string callSetupListen(int calledParty);

    /**
     * Connects a call and starts the voice flow.
     *
     * @param[in]  ssi     Calling party ISSI.
     * @param[out] audKey  Audio RTP crypto key. 0 if no encryption.
     * @param[out] vidPort Video RTP local port number. 0 if not a video call.
     * @param[out] vidKey  Video RTP crypto key. 0 if not a video call or no
     *                     encryption.
     * @return The audio RTP local port number, or 0 on failure.
     */
    int callConnect(int          ssi,
                    std::string *audKey = 0,
                    int         *vidPort = 0,
                    std::string *vidKey = 0);

    /**
     * Releases a call.
     *
     * @param[in] ssi The called party ISSI/GSSI.
     * @return true if successful.
     */
    bool callRelease(int ssi);

    static void setAcelp(bool enable) { sAcelp = enable; }

    static bool getAcelp() { return sAcelp; }

private:
    int mSessionId;   //incrementing session ID
    int mLocalRtpPort;

    static bool sAcelp; //ACELP enabled/disabled

    /**
     * Makes a call.
     *
     * @param[in] calledParty  Called number.
     * @param[in] callingParty Calling number.
     * @param[in] calledDomain The VOIP called party domain for dispatcher call.
     *                         Empty string to use the server IP.
     * @param[in] isVideo      true if a video call.
     * @param[in] numPrefix    The number prefix, if needed.
     *                         See VoipSessionBase::NUMPREFIX_*.
     * @param[in] params       Call parameters, if any.
     * @return The call ID.
     */
    std::string callSetup(int                calledParty,
                          int                callingParty,
                          const std::string &calledDomain,
                          bool               isVideo,
                          char               numPrefix = 0,
                          const std::string &params = "");

    /**
     * Processes a VOIP server message.
     *
     * @param[in]  msg   The VOIP server message.
     * @param[out] msgSp The callback message, if any.
     * @return The response message, or 0 if none. Caller takes ownership and is
     *         responsible for deleting it.
     */
    MsgSip *processMsg(const MsgSip *msg, MsgSp *&msgSp);

    /**
     * Gets next session ID.
     *
     * @return New session ID.
     */
    int getNewSessionId();

    /**
     * Gets the next local RTP port number.
     *
     * @return The port number.
     */
    int getLocalRtpPort();

    /**
     * Generates a random local crypto key.
     *
     * @return The key.
     */
    std::string getLocalCryptoKey();

    /**
     * Encodes a binary buffer to a Base64-encoded string.
     *
     * @param[in] buf Buffer to be encoded.
     * @param[in] len Buffer length.
     * @return The encoded string.
     */
    std::string base64Encode(const unsigned char *buf, int len);
};
#endif //VOIPSESSIONCLIENT_H
