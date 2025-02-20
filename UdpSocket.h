/**
 * UDP socket services.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2014. All Rights Reserved.
 *
 * @file
 * @version $Id: UdpSocket.h 142 2014-01-16 04:21:53Z hzahari $
 * @author Mohd Rashid
 * @author Mohd Rozaimi
 */
#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "Socket.h"

class UdpSocket : public Socket
{
public:
    /**
     * Constructor.
     *
     * @param[in] remotePort The remote host port number.
     * @param[in] remoteIp   The remote host IP.
     * @param[in] localPort  The local port number. 0 to use any available.
     */
    UdpSocket(int                remotePort,
              const std::string &remoteIp = LOCALHOST,
              int                localPort = 0);

    using Socket::send; //overloading base class send()

    /**
     * Sends data through the socket to the given remote host.
     * The specified host IP and port are sticky, which means they replace
     * the current stored values, and the next call to send(data, len) will
     * use the same destination.
     *
     * @param[in] data       The data buffer.
     * @param[in] len        The data length in bytes.
     * @param[in] remoteIp   The remote host IP.
     * @param[in] remotePort The remote host port number.
     * @return The number of bytes sent, or ERR_INVALID_REMOTE_IP,
     *         or a negative errno on failure.
     */
    int send(const char        *data,
             int                len,
             const std::string &remoteIp,
             int                remotePort);

    /**
     * Sends a message through the socket to the given remote host.
     * The specified host IP and port are sticky, which means they replace
     * the current stored values, and the next call to send(msg) will use the
     * same destination.
     *
     * @param[in] msg        The message.
     * @param[in] remoteIp   The remote host IP.
     * @param[in] remotePort The remote host port number.
     * @return The number of bytes sent, or ERR_INVALID_REMOTE_IP,
     *         or a negative errno on failure.
     */
    int send(const std::string &msg,
             const std::string &remoteIp,
             int                remotePort);
};
#endif //UDPSOCKET_H
