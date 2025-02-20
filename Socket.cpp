/**
 * Base socket class implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Socket.cpp 1809 2024-02-02 06:43:13Z zulzaidi $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#include <cstring>    //memset
#include <sstream>

#include "Socket.h"

using namespace std;

//static initializers
#ifdef IPV6
const string Socket::LOCALHOST("::1");
#else
const string Socket::LOCALHOST("127.0.0.1");
#endif
bool       Socket::sInitialized(PalSocket::init());
set<SSL *> Socket::sValidSsls;

Socket::~Socket()
{
    close();
}

int Socket::setNonblocking()
{
    if (PalSocket::setNonblocking(mSock) != 0)
        return -PalSocket::getError();
    return 0;
}

int Socket::connect()
{
    if (!mHasRemoteAddress)
        return ERR_MISSING_REMOTE_ADDR;
    if (mIsConnected)
        close();
    if (mSock == INVALID_SOCKET && createSocket((mType == TYPE_UDP)) != 0)
        return -PalSocket::getError();
    //connect() returns 0 on success, -1 otherwise, with errno
    if (::connect(mSock, (sockaddr *)&mRemoteAddr, sizeof(mRemoteAddr)) != 0)
        return -PalSocket::getError();
    mIsConnected = true;
    if (mUseSsl)
    {
        SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
        if (ctx != 0)
            mSsl = SSL_new(ctx);
        int ret;
        if (mSsl == 0)
        {
            if (ctx != 0)
                SSL_CTX_free(ctx);
            ret = ERR_INIT_SSL;
        }
        else
        {
            SSL_set_fd(mSsl, mSock);
            ret = SSL_connect(mSsl);
            if (ret <= 0)
                ret = -PalSocket::getError(mSsl, ret);
        }
        if (ret < 0)
        {
            close();
            return ret;
        }
        sValidSsls.insert(mSsl);
    }
    //get the local IP address and port
    socklen_t len = sizeof(mLocalAddr);
    if (getsockname(mSock, (sockaddr *)&mLocalAddr, &len) == 0)
    {
        mLocalPort = getPort(mLocalAddr);
        mLocalIp = getIp(mLocalAddr);
    }
    return 0;
}

int Socket::connect(const string &remoteIp, int remotePort)
{
    if (!setRemoteAddr(remoteIp, remotePort))
        return ERR_INVALID_REMOTE_IP;
    return connect();
}

int Socket::send(const char *data, int len, const string *ip, int port)
{
    if (mSock == INVALID_SOCKET)
        return ERR_INVALID_SOCKET;
    int ret;
    if (mIsConnected)
    {
        //SSL_write() and send() return the number of bytes sent on success,
        //<0 otherwise, with errno
        if (mSsl != 0)
            ret = SSL_write(mSsl, data, len);
        else
            ret = PalSocket::send(mSock, data, len);
    }
    else if (mType == TYPE_TCP)
    {
        return ERR_INVALID_SOCKET; //just got disconnected
    }
    else if (ip == 0)
    {
        if (!mHasRemoteAddress)
            return ERR_MISSING_REMOTE_ADDR;
        //sendto() returns the number of bytes sent on success,
        //-1 otherwise, with errno
        ret = ::sendto(mSock, data, len, 0, (struct sockaddr *)&mRemoteAddr,
                       sizeof(struct sockaddr));
    }
    else
    {
        SockAddrT addr;
        if (!getAddress(*ip, addr))
            return ERR_INVALID_REMOTE_IP;
        setPort(addr, (port != 0)? port: mRemotePort, true);
        ret = ::sendto(mSock, data, len, 0, (struct sockaddr *)&addr,
                       sizeof(struct sockaddr));
    }
    if (ret < 0)
        ret = (mSsl != 0)? -PalSocket::getError(mSsl, ret):
                           -PalSocket::getError();
    return ret;
}

int Socket::send(const string &msg, const string *ip, int port)
{
    return send(msg.c_str(), msg.length(), ip, port);
}

int Socket::recv(char   *buf,
                 int     bufSize,
                 int     timeout,
                 string *remoteIp,
                 int    *remotePort)
{
    if (mSock == INVALID_SOCKET)
        return ERR_INVALID_SOCKET;
    int ret = 1; //init to any positive
    if (timeout > 0)
    {
        ret = PalSocket::pollFd(mSock, timeout);
        if (ret == 0)
        {
            if (mSock == INVALID_SOCKET) //closed
                return ERR_INVALID_SOCKET;
            if (timeout <= 0)
                return ERR_TIMEOUT;
        }
    }
    if (ret > 0)
    {
        if (!mIsConnected)
        {
            //recvfrom() returns the number of bytes received on success,
            //-1 on failure, with errno
            SockAddrT remoteAddr;
            socklen_t len = sizeof(struct sockaddr);
            ret = ::recvfrom(mSock, buf, bufSize, 0,
                             (struct sockaddr *)&remoteAddr, &len);
            if (ret > 0)
            {
                if (remoteIp != 0)
                    *remoteIp = getIp(remoteAddr);
                if (remotePort != 0)
                    *remotePort = getPort(remoteAddr);
            }
        }
        //SSL_read() and recv() return the number of bytes received on success,
        //0 if the remote host has closed the connection,
        //<0 on failure, with errno
        else if (mSsl != 0)
        {
            ret = SSL_read(mSsl, buf, bufSize);
        }
        else
        {
            ret = ::recv(mSock, buf, bufSize, 0);
        }
    }
    if (ret < 0)
        ret = (mSsl == 0)? -PalSocket::getError():
                           (sValidSsls.count(mSsl) != 0)?
                               -PalSocket::getError(mSsl, ret):
                               ERR_INVALID_SOCKET;
    return ret;
}

void Socket::close()
{
    if (mSock != INVALID_SOCKET)
    {
        PalSocket::close(mSock);
        mSock = INVALID_SOCKET;
    }
    if (mSsl != 0)
    {
        sValidSsls.erase(mSsl);
        SSL_shutdown(mSsl);
        SSL_CTX_free(SSL_get_SSL_CTX(mSsl));
        SSL_free(mSsl);
        mSsl = 0;
    }
    mIsConnected = false;
}

bool Socket::setRemoteAddr(const string &remoteIp, int remotePort)
{
    mHasRemoteAddress = getAddress(remoteIp, mRemoteAddr);
    if (mHasRemoteAddress)
    {
        mRemoteIp = remoteIp;
        if (remotePort > 0)
            mRemotePort = remotePort;
        setPort(mRemoteAddr, mRemotePort);
    }
    return mHasRemoteAddress;
}

string Socket::getLocalAddrStr() const
{
    std::ostringstream oss;
    oss << mLocalIp << ':' << mLocalPort;
    return oss.str();
}

string Socket::getRemoteAddrStr() const
{
    std::ostringstream oss;
    oss << mRemoteIp << ':' << mRemotePort;
    return oss.str();
}

bool Socket::isValid() const
{
    return (sInitialized && mSock != INVALID_SOCKET);
}

bool Socket::validateIp(const string &ip)
{
    if (ip.empty())
        return false;
    string    ipAddr;
    SockAddrT addr;
    std::istringstream iss(ip);
    bool retVal = false;
    while (iss >> ipAddr)
    {
        retVal = getAddress(ipAddr, addr);
        if (!retVal)
            break;
    }
    return retVal;
}

string Socket::getIp(const SockAddrT &addr)
{
    return PalSocket::getIp(addr);
}

bool Socket::getAddress(const string &ip, SockAddrT &addr)
{
    return PalSocket::getAddress(ip, addr);
}

string Socket::getErrorStr(int code)
{
    if (code == 0)
        return "";
    return string(" '").append(PalSocket::getErrorStr(code)).append("'");
}

Socket::Socket(int type) :
mSock(INVALID_SOCKET), mType(type), mLocalPort(0), mRemotePort(0),
mIsConnected(false), mHasRemoteAddress(false), mUseSsl(false), mSsl(0)
{
}

Socket::Socket(int type, const string &remoteIp, int remotePort, bool useSsl) :
mSock(INVALID_SOCKET), mType(type), mLocalPort(0), mRemotePort(remotePort),
mRemoteIp(remoteIp), mIsConnected(false), mUseSsl(mType == TYPE_TCP && useSsl),
mSsl(0)
{
    memset(&mLocalAddr, 0, sizeof(mLocalAddr));
    memset(&mRemoteAddr, 0, sizeof(mRemoteAddr));
    setPort(mRemoteAddr, remotePort, true);
    mHasRemoteAddress = getAddress(mRemoteIp, mRemoteAddr);
}

int Socket::createSocket(bool doBind)
{
    //socket() returns a positive integer descriptor on success,
    //-1 otherwise, with errno
#ifdef IPV6
    mSock = socket(AF_INET6, mType, 0);
#else
    mSock = socket(AF_INET, mType, 0);
#endif //IPV6
    if (mSock < 0)
        return -PalSocket::getError();

    PalSocket::setOpt(mSock);

    if (doBind)
    {
        memset(&mLocalAddr, 0, sizeof(mLocalAddr));
#ifdef IPV6
        mLocalAddr.sin6_addr = in6addr_any;
#else
        mLocalAddr.sin_addr.s_addr = INADDR_ANY;
#endif //IPV6
        setPort(mLocalAddr, mLocalPort, true);
        //bind() returns 0 on success, -1 otherwise, with errno
        if (::bind(mSock, (sockaddr *)&mLocalAddr, sizeof(mLocalAddr)) != 0)
        {
            //save the error code first because close() may change it
            int err = -PalSocket::getError();
            close();
            return err;
        }
        mLocalIp = getIp(mLocalAddr);
        if (mLocalPort == 0)
        {
            //get the system-assigned local port
            socklen_t len = sizeof(mLocalAddr);
            if (getsockname(mSock, (sockaddr *)&mLocalAddr, &len) == 0)
                mLocalPort = getPort(mLocalAddr);
        }
    }
    return 0;
}

inline void Socket::setPort(SockAddrT &addr, int port, bool doSetFamily)
{
    if (doSetFamily)
#ifdef IPV6
        addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
#else
        addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
#endif
}

inline int Socket::getPort(const SockAddrT &addr) const
{
#ifdef IPV6
    if (addr.sin6_family == AF_INET6)
        return ntohs(addr.sin6_port);
#else
    if (addr.sin_family == AF_INET)
        return ntohs(addr.sin_port);
#endif
    return 0;
}
