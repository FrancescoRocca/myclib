#ifndef MYCLIB_SOCKET_H
#define MYCLIB_SOCKET_H

#ifdef _WIN32
/* Windows */
#include <winsock2.h>
#include <ws2tcpip.h>
#else
/* Unix */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#endif

/* Run this before everything */
int sock_platform_init();

/* Use this to close a socket */
int sock_close(int socket);

/* Use at exit */
int sock_platform_shutdown();

#endif
