#ifndef MYCLIB_SOCKET_H
#define MYCLIB_SOCKET_H

#ifdef _WIN32
/* Windows */
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#else
/* Unix */

#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K 1
#endif

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#endif

/* Run this before everything */
int sock_platform_init();

/* Use this to close a socket */
int sock_close(int socket);

/* Read/Write all to socket */
int sock_readall(int socket, void *out, size_t n);
int sock_writeall(int socket, const void *buf, size_t n);

/* Use at exit */
int sock_platform_shutdown();

#endif
