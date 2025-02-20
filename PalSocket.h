/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific socket implementation.
 *
 * @remarks
 *     On Windows:
 *     1. The build may require #define WIN32_LEAN_AND_MEAN to prevent
 *        windows.h from including winsock.h. Otherwise including WinSock2.h
 *        here may cause redefinition errors.
 *        For example, in QT .pro file, add:
 *            DEFINES += WIN32_LEAN_AND_MEAN
 *     2. At application exit, call PalSocket::finalize() for proper socket
 *        shutdown.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @version $Id: PalSocket.h 1765 2023-10-09 02:58:23Z zulzaidi $
 * @author Mohd Rozaimi
 */
#ifndef PAL_SOCKET_H
#define PAL_SOCKET_H

#include <openssl/ssl.h>
#include <string>

#if defined(_WIN32) || defined(WIN32) //Windows ===========================
#include <WinSock2.h>
#include <ws2tcpip.h>   //INET_ADDRSTRLEN, inet_ntop/pton, socklen_t

//link WinSock2 lib
#pragma comment(lib, "Ws2_32.lib")

//WinSock2.h defines INVALID_SOCKET

typedef SOCKET SocketT;
#ifdef IPV6
typedef sockaddr_in6 SockAddrT;
#else
typedef sockaddr_in  SockAddrT;
#endif

namespace PalSocket
{
    inline bool init()
    {
        WSADATA wsaData;
        //highest version now is 2.2
        return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
    }

    inline void finalize()
    {
        WSACleanup();
    }

    inline void setOpt(SocketT sock)
    {
        char on = '1';
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
    }

    inline int setNonblocking(SocketT sock)
    {
        unsigned long mode = 1;
        return ioctlsocket(sock, FIONBIO, &mode);
    }

    inline std::string getIp(const SockAddrT &addr)
    {
#ifdef IPV6
        char ip[INET6_ADDRSTRLEN] = {0};
        //inet_ntop() returns NULL on error
        inet_ntop(AF_INET6, (void *)(&addr.sin6_addr), ip, sizeof(ip));
#else
        char ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, (void *)(&addr.sin_addr), ip, sizeof(ip));
#endif //IPV6
        return std::string(ip);
    }

    inline int send(SocketT sockfd, const char *data, int len)
    {
        return ::send(sockfd, data, len, 0);
    }

    inline void close(SocketT sock)
    {
        closesocket(sock);
    }

    inline int getError()
    {
        return WSAGetLastError();
    }

    inline int getError(SSL *ssl, int code)
    {
        switch (SSL_get_error(ssl, code))
        {
            case SSL_ERROR_SYSCALL:
                return WSAGetLastError(); //lower-level I/O error, use WSA error
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return WSAEWOULDBLOCK;
            default:
                return WSAENOTCONN; //generic error
        }
    }

    inline std::string getErrorStr(int code)
    {
        LPSTR msg = NULL; //to be allocated and filled by FormatMessageA
        int   len = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                   FORMAT_MESSAGE_FROM_SYSTEM,
                                   NULL, code, 0, (LPSTR)&msg, 0, NULL);
        std::string str;
        if (len > 1)
            str.assign(msg, len - 1); //discard the ending newline
        LocalFree(msg);
        return str;
    }

    inline bool isDisconnectedError(int code)
    {
        switch (code)
        {
            case 0:
            case WSAENETDOWN:
            case WSAENETUNREACH:
            case WSAENETRESET:
            case WSAECONNABORTED:
            case WSAECONNRESET:
            case WSAEHOSTDOWN:
            case WSAEHOSTUNREACH:
                return true;
            default:
                return false;
        }
    }

    inline bool isWouldBlockError(int code)
    {
        return (code == WSAEWOULDBLOCK);
    }

    inline int pollFd(const SocketT &fd, int &timeout)
    {
        int ret = 0;
        fd_set rfds;
        struct timeval tv;
        //break timeout into shorter chunks to be able to detect closed socket
        while (timeout > 0 && ret == 0 && fd != INVALID_SOCKET)
        {
            tv.tv_sec = (timeout < 4)? timeout: 4;
            timeout -= tv.tv_sec;
            tv.tv_usec = 0;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            ret = select(fd + 1, &rfds, NULL, NULL, &tv); //ret 0 means timeout
        }
        return ret;
    }
} //namespace PalSocket

#else  //linux ============================================================
#include <errno.h>
#include <fcntl.h>
#include <string.h>     //strerror
#include <unistd.h>     //::close
#include <arpa/inet.h>  //INET_ADDRSTRLEN, sockaddr_in, htons, inet_ntop/pton
#include <sys/poll.h>
#include <sys/socket.h>

#define INVALID_SOCKET -1

typedef int SocketT;
#ifdef IPV6
typedef sockaddr_in6 SockAddrT;
#else
typedef sockaddr_in  SockAddrT;
#endif

namespace PalSocket
{
    inline bool init()
    {
        return true; //do nothing
    }

    inline void finalize()
    {
        //do nothing
    }

    inline void setOpt(SocketT sock)
    {
        int on = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
    }

    inline int setNonblocking(SocketT sock)
    {
        return fcntl(sock, F_SETFL, O_NONBLOCK);
    }

    inline std::string getIp(const SockAddrT &addr)
    {
#ifdef IPV6
        char ip[INET6_ADDRSTRLEN] = {0};
        //inet_ntop() returns NULL on error
        inet_ntop(AF_INET6, &addr.sin6_addr, ip, sizeof(ip));
#else
        char ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
#endif //IPV6
        return std::string(ip);
    }

    inline int send(SocketT sockfd, const void *data, int len)
    {
        return ::send(sockfd, data, len, MSG_NOSIGNAL);
    }

    inline void close(SocketT sock)
    {
        ::close(sock);
    }

    inline int getError()
    {
        return errno;
    }

    inline int getError(SSL *ssl, int code)
    {
        switch (SSL_get_error(ssl, code))
        {
            case SSL_ERROR_SYSCALL:
                return errno; //lower-level I/O error, use socket error
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return EWOULDBLOCK;
            default:
                return ENOTCONN; //generic error
        }
    }

    inline std::string getErrorStr(int code)
    {
        return std::string(strerror(code));
    }

    inline bool isDisconnectedError(int code)
    {
        switch (code)
        {
            case 0:
            case ECONNABORTED:
            case ECONNRESET:
            case EHOSTDOWN:
            case EHOSTUNREACH:
            case ENETDOWN:
            case ENETRESET:
            case ENETUNREACH:
            case ENOLINK:
            case ENONET:
                return true;
            default:
                return false;
        }
    }

    inline bool isWouldBlockError(int code)
    {
        return (code == EWOULDBLOCK);
    }

    inline int pollFd(const SocketT &fd, int &timeout)
    {
        int ret = 0;
        int t; //poll timeout in ms
        struct pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = POLLIN;
        //break timeout into shorter chunks to be able to detect closed socket
        while (timeout > 0 && ret == 0 && fd != INVALID_SOCKET)
        {
            t = (timeout < 4)? timeout: 4;
            timeout -= t;
            ret = poll(fds, 1, t * 1000); //ret 0 means timeout
        }
        return ret;
    }
} //namespace PalSocket

#endif //WIN32

//common ==================================================================
namespace PalSocket
{
    inline bool getAddress(const std::string &ip, SockAddrT &addr)
    {
        //inet_pton() returns 1 on success, 0 if the address string cannot be
        //converted, and -1 if sin_family is not supported (should never
        //occur)
#ifdef IPV6
        return (inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr) == 1);
#else
        return (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) == 1);
#endif //IPV6
    }
} //namespace PalSocket
#endif //PAL_SOCKET_H
