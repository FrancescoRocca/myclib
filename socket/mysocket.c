#include "mysocket.h"

int sock_platform_init() {
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

int sock_close(int socket) {
	int ret = 0;

#ifdef _WIN32
	ret = closesocket(socket);
#else
	ret = close(socket);
#endif

	return ret;
}

int sock_platform_shutdown() {
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

int sock_readall(int socket, void *out, size_t n) {
	char *p = (char *)out;
	size_t bytes_to_read = n;
	int bytes_read = 0;

	while (bytes_to_read > 0) {
		bytes_read = recv(socket, p, bytes_to_read, 0);

		if (bytes_read > 0) {
			p += bytes_read;
			bytes_to_read -= bytes_read;
		} else if (bytes_read == 0) {
			break;
		} else {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
	}

	return n - bytes_to_read;
}

int sock_writeall(int socket, const void *buf, size_t n) {
	const char *p = (char *)buf;
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
			}

			return -1;
		}
	}

	return n;
}
