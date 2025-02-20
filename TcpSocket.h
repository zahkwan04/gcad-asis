/**
 * TCP socket services.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: TcpSocket.h 1765 2023-10-09 02:58:23Z zulzaidi $
 * @author Zahari Hadzir
 * @author Mohd Rozaimi
 */
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "Socket.h"

class TcpSocket : public Socket
{
public:
    /**
     * Constructor for a client socket.
     *
     * @param[in] remoteIp   The remote host IP.
     * @param[in] remotePort The remote host port number.
     * @param[in] useSsl     true to enable SSL.
     */
    TcpSocket(const std::string &remoteIp, int remotePort, bool useSsl = false);

    /**
     * Constructor for a server socket.
     * Expects a socket descriptor for a connected socket, or a local port
     * number for a listening socket.
     *
     * @param[in] sock The socket descriptor, or 0 for a listening socket.
     * @param[in] port The local port number. Omit for a connected socket.
     */
    TcpSocket(SocketT sock, int port = 0);

    /**
     * Starts listening for client connections.
     *
     * @return 0 if successful, or Socket::ERR_INVALID_SOCKET or a negative
     *         errno on error.
     */
    int listen();

    /**
     * Listens for and accepts a client connection.
     * Blocks until connection received.
     *
     * @param[out] remoteAddr The remote host address.
     * @param[out] remotePort The remote host port number.
     * @return The accepted socket descriptor, or INVALID_SOCKET or a
     *         negative errno on error.
     */
    SocketT accept(std::string &remoteAddr, int &remotePort);

    /**
     * Closes the socket.
     */
    void close();

private:
    bool mIsListening;
};
#endif //TCPSOCKET_H
