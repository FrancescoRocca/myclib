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
