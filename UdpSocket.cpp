/**
 * UDP socket services implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013. All Rights Reserved.
 *
 * @file
 * @version $Id: UdpSocket.cpp 81 2013-11-06 10:18:00Z syukri $
 * @author Mohd Rashid
 * @author Mohd Rozaimi
 */
#include "PalSocket.h"
#include "UdpSocket.h"

using std::string;

UdpSocket::UdpSocket(int remotePort, const string &remoteIp, int localPort) :
Socket(TYPE_UDP, remoteIp, remotePort)
{
    mLocalPort = localPort;
    createSocket(true);
}

int UdpSocket::send(const char   *data,
                    int           len,
                    const string &remoteIp,
                    int           remotePort)
{
    if (!mHasRemoteAddress || remotePort != mRemotePort ||
        remoteIp != mRemoteIp)
    {
        if (!setRemoteAddr(remoteIp, remotePort))
            return ERR_INVALID_REMOTE_IP;
    }
    return Socket::send(data, len);
}

int UdpSocket::send(const string &msg,
                    const string &remoteIp,
                    int           remotePort)
{
    return send(msg.c_str(), msg.length(), remoteIp, remotePort);
}
