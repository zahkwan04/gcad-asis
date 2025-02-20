/**
 * Base socket class.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Socket.h 1807 2024-01-30 05:37:50Z zulzaidi $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#ifndef SOCKET_H
#define SOCKET_H

#include <openssl/ssl.h>
#include <set>
#include <string>

#include "PalSocket.h"

class Socket
{
public:
    enum eError
    {
        ERR_INVALID_SOCKET          = -90000,
        ERR_TIMEOUT                 = -90001,
        ERR_MISSING_REMOTE_ADDR     = -90002,
        ERR_INVALID_REMOTE_ADDR     = -90003,
        ERR_INVALID_REMOTE_IP       = -90004,
        ERR_INIT_SSL                = -90005
    };

    static const std::string LOCALHOST;

    virtual ~Socket();

    /**
     * Sets this socket to be non-blocking.
     *
     * @return 0 if successful, or a negative errno.
     */
    int setNonblocking();

    /**
     * Connects to a host with the currently configured IP and port.
     * Closes any existing connection first.
     *
     * @return 0 if successful, or ERR_MISSING_REMOTE_ADDR, ERR_INIT_SSL,
     *         or a negative errno.
     */
    int connect();

    /**
     * Connects to a host with a new IP, and optionally new port number.
     * Closes any existing connection first.
     *
     * @param[in] remoteIp   The remote host IP.
     * @param[in] remotePort The remote host port number. Omit to use current
     *                       configured port.
     * @return 0 if successful, or ERR_INVALID_REMOTE_IP,
     *         or a negative errno.
     */
    int connect(const std::string &remoteIp, int remotePort = 0);

    /**
     * Sends data through the socket.
     *
     * @param[in] data The data buffer.
     * @param[in] len  The data length in bytes.
     * @param[in] ip   (UDP only) The remote IP address.
     *                 Omit to use the current one.
     * @param[in] port (UDP only) The remote port number.
     *                 Omit to use the current one.
     * @return The number of bytes sent, or ERR_INVALID_SOCKET,
     *         ERR_MISSING_REMOTE_ADDR, or a negative errno on failure.
     */
    int send(const char *data,
             int         len,
             const       std::string *ip = 0,
             int         port = 0);

    /**
     * Sends a message through the socket.
     *
     * @param[in] msg  The message string.
     * @param[in] ip   (UDP only) The remote IP address.
     *                 Omit to use the current one.
     * @param[in] port (UDP only) The remote port number.
     *                 Omit to use the current one.
     * @return The number of bytes sent, or ERR_INVALID_SOCKET,
     *         ERR_MISSING_REMOTE_ADDR, or a negative errno on failure.
     */
    int send(const std::string &msg,
             const std::string *ip = 0,
             int                port = 0);

    /**
     * Receives data from a socket. Blocks until data received or timeout.
     *
     * @param[out] buf        The data buffer.
     * @param[in]  bufSize    The buffer size in bytes.
     * @param[in]  timeout    The timeout in seconds, or 0 for no timeout.
     * @param[out] remoteIp   (UDP only) The remote IP address if needed.
     * @param[out] remotePort (UDP only) The remote port number if needed.
     * @return The number of bytes received, or:
     *         - ERR_INVALID_SOCKET,
     *         - ERR_TIMEOUT,
     *         - a negative errno on failure,
     *         - 0 if the remote host has closed the connection.
     */
    int recv(char        *buf,
             int          bufSize,
             int          timeout = 0,
             std::string *remoteIp = 0,
             int         *remotePort = 0);

    /**
     * Closes the socket, making it invalid, and frees SSL context if any.
     */
    void close();

    /**
     * Sets the remote host IP and port number.
     *
     * @param[in] remoteIp   The remote host IP.
     * @param[in] remotePort The remote host port number. Omit to use current
     *                       configured port.
     * @return true if successful, false if IP is invalid.
     */
    bool setRemoteAddr(const std::string &remoteIp, int remotePort = 0);

    const std::string &getLocalIp() const { return mLocalIp; }

    const std::string &getRemoteIp() const { return mRemoteIp; }

    /**
     * Gets the local address in the form "IP:Port".
     *
     * @return The local address.
     */
    std::string getLocalAddrStr() const;

    /**
     * Gets the remote address in the form "IP:Port".
     *
     * @return The remote address.
     */
    std::string getRemoteAddrStr() const;

    SocketT getSock() const { return mSock; }

    int getLocalPort() const { return mLocalPort; }

    int getRemotePort() const { return mRemotePort; }

    /**
     * Checks whether the socket is valid and ready to use.
     *
     * @return true if valid.
     */
    bool isValid() const;

    /**
     * Validates one or more IP address strings.
     *
     * @param[in] ip Space-separated IP addresses.
     * @return true if all IP addresses are valid, false otherwise.
     */
    static bool validateIp(const std::string &ip);

    /**
     * Converts an IPv4 or IPv6 address from binary to IP string form.
     *
     * @param[in] addr The binary address container.
     * @return The IP string.
     */
    static std::string getIp(const SockAddrT &addr);

    /**
     * Converts an IPv4 or IPv6 address from IP string to binary form.
     *
     * @param[in]  ip   The IP string.
     * @param[out] addr The binary address container.
     * @return true if successful.
     */
    static bool getAddress(const std::string &ip, SockAddrT &addr);

    /**
     * Gets the system error string for an error code.
     *
     * @param[in] code The error code.
     * @return The error string if any, or empty string otherwise.
     *         The error string is enclosed in single quotes and prefixed
     *         with a space.
     */
    static std::string getErrorStr(int code);

    /**
     * Checks whether a socket error code indicates disconnection.
     *
     * @param[in] code The error code.
     * @return true for disconnection.
     */
    static bool isDisconnectedError(int code)
    {
        return PalSocket::isDisconnectedError(code);
    }

    static bool isWouldBlockError(int code)
    {
        return PalSocket::isWouldBlockError(code);
    }

protected:
    enum eType
    {
        TYPE_TCP = SOCK_STREAM,
        TYPE_UDP = SOCK_DGRAM
    };

    SocketT      mSock;              //socket descriptor
    int          mType;
    int          mLocalPort;
    int          mRemotePort;
    std::string  mLocalIp;
    std::string  mRemoteIp;
    bool         mIsConnected;
    bool         mHasRemoteAddress;
    bool         mUseSsl;
    SockAddrT    mLocalAddr;
    SockAddrT    mRemoteAddr;
    SSL         *mSsl;

    static bool            sInitialized;
    static std::set<SSL *> sValidSsls;

    /**
     * Constructor for server socket.
     *
     * @param[in] type The socket type.
     */
    Socket(int type);

    /**
     * Constructor for client socket.
     *
     * @param[in] type       The socket type.
     * @param[in] remoteIp   The remote host IP string.
     * @param[in] remotePort The remote host port number.
     * @param[in] useSsl     true to enable SSL/TLS for TYPE_TCP.
     */
    Socket(int                type,
           const std::string &remoteIp,
           int                remotePort,
           bool               useSsl = false);

    /**
     * Creates the socket object.
     *
     * @param[in] doBind true to bind the socket to the local address.
     * @return 0 if successful, or a negative errno otherwise.
     */
    int createSocket(bool doBind);

    /**
     * Sets the port number in an IP address container.
     *
     * @param[in,out] addr        The address container.
     * @param[in]     port        The port number.
     * @param[in]     doSetFamily true to set the address family.
     */
    void setPort(SockAddrT &addr, int port, bool doSetFamily = false);

    /**
     * Gets the port number from an IP address container.
     *
     * @param[in] addr The address container.
     * @return The port number, or 0 if the address is not of the
     *         AF_INET/AF_INET6 family.
     */
    int getPort(const SockAddrT &addr) const;
};
#endif //SOCKET_H
