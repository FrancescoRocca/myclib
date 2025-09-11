#include <stdio.h>

#include "../../socket/mysocket.h"

void test_socket1() {
	sock_platform_init();

	struct addrinfo hints, *res, *p;
	char ipstr[INET6_ADDRSTRLEN];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char hostname[1024];
	printf("hostname> ");
	fscanf(stdin, "%s", hostname);

	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		fprintf(stderr, "getaddrinfo()\n");
		sock_platform_shutdown();
	}

	for (p = res; p != NULL; p = p->ai_next) {
		void *addr = 0;
		char *ipver = 0;
		struct sockaddr_in *ipv4;

		if (p->ai_family == AF_INET) {
			ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else {
			continue;
		}
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("%s: %s\n", ipver, ipstr);
	}

	sock_platform_shutdown();
}
