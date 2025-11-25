#ifndef MYCLIB_SOCKET_H
#define MYCLIB_SOCKET_H

#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socket_t;
#else
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#endif

/* Initialize the socket system */
int sock_platform_init(void);

/* Close a socket */
int sock_close(socket_t socket);

/* Clean the socket system */
int sock_platform_shutdown(void);

/*
 * Read 'bufsize' bytes from socket
 * Returns the read bytes, -1 on failure, 0 connection closed without issues
 */
int sock_readall(socket_t sockfd, void *buf, size_t bufsize);

/*
 * Writes 'n' bytes to socket
 */
int sock_writeall(socket_t socket, const void *buf, size_t n);

#endif
