/**
 * TCP socket services implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: TcpSocket.cpp 1765 2023-10-09 02:58:23Z zulzaidi $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#include "PalSocket.h"
#include "TcpSocket.h"

using std::string;

TcpSocket::TcpSocket(const string &remoteIp, int remotePort, bool useSsl) :
Socket(TYPE_TCP, remoteIp, remotePort, useSsl), mIsListening(false)
{
}

TcpSocket::TcpSocket(SocketT sock, int port) :
Socket(TYPE_TCP), mIsListening(false)
{
    mLocalPort = port;
    if (sock > 0)
    {
        mSock = sock;           //accepted socket
        mIsConnected = true;
    }
    else if (port > 0)
    {
        createSocket(true);     //listening socket
    }
}

int TcpSocket::listen()
{
    if (!mIsListening)
    {
        if (mSock == INVALID_SOCKET)
            return ERR_INVALID_SOCKET;
        //listen() returns 0 on success, -1 otherwise, with errno
        if (::listen(mSock, 128) != 0)
            return -PalSocket::getError();
        mIsListening = true;
    }
    return 0;
}

SocketT TcpSocket::accept(string &remoteAddr, int &remotePort)
{
    if (mSock == INVALID_SOCKET)
        return mSock;
    if (!mIsListening)
    {
        int val = listen();
        if (val != 0)
            return val;
    }
    socklen_t addrLen = sizeof(mRemoteAddr);
    //accept() returns a new descriptor for the accepted socket, or
    //INVALID_SOCKET on error, with errno
    SocketT sock = ::accept(mSock, (struct sockaddr *)&mRemoteAddr, &addrLen);
    if (sock == INVALID_SOCKET)
        return -PalSocket::getError();
    remoteAddr = Socket::getIp(mRemoteAddr);
#ifdef IPV6
    remotePort = ntohs(mRemoteAddr.sin6_port);
#else
    remotePort = ntohs(mRemoteAddr.sin_port);
#endif
    return sock;
}

void TcpSocket::close()
{
    Socket::close();
    mIsListening = false;
}
