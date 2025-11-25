#include "mysocket.h"
#include <errno.h>

int sock_platform_init(void) {
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return -1;
	}
#endif
	return 0;
}

int sock_close(socket_t socket) {
	int ret = 0;
#ifdef _WIN32
	ret = closesocket(socket);
#else
	ret = close(socket);
#endif
	return ret;
}

int sock_platform_shutdown(void) {
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}

int sock_readall(socket_t sockfd, void *buf, size_t bufsize) {
	char *p = (char *)buf;
	size_t total_read = 0;

	while (total_read < bufsize) {
		int n = recv(sockfd, p, bufsize - total_read, 0);

		if (n > 0) {
			/* Data received */
			p += n;
			total_read += n;
		} else if (n == 0) {
			/* Connection closed */
			break;
		} else {
			/* Error */
			if (errno == EINTR) {
				/* Try again */
				continue;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				/* Socket non-blocking, no data right now */
				/* Returns what has read */
				return total_read;
			} else {
				return -1;
			}
		}
	}

	return total_read;
}

int sock_writeall(socket_t socket, const void *buf, size_t n) {
	const char *p = (const char *)buf;
	size_t bytes_to_write = n;
	int bytes_written;

	while (bytes_to_write > 0) {
		bytes_written = send(socket, p, bytes_to_write, 0);

		if (bytes_written >= 0) {
			p += bytes_written;
			bytes_to_write -= bytes_written;
		} else {
			if (errno == EINTR) {
				continue;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return n - bytes_to_write;
			} else {
				return -1;
			}
		}
	}

	return n;
}
