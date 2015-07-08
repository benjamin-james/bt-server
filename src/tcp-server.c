#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uds.h"
#include "tcp.h"

#define MAX_CLIENTS 32

void handler(int sock, void *data, size_t size)
{
	char buffer[256];
	int udssock;
	ssize_t len;
	memcpy(buffer, data, size);
	udssock = uds_client_connect(buffer);
	while ((len = recv(sock, buffer, sizeof(buffer), 0)) >= 0) {
		send(udssock, buffer, len, 0);
	}
	close(udssock);
}
int main(int argc, char **argv)
{
	if (argc < 3 || sscanf(argv[1], "%d", &argc) != 1 || access(argv[2], F_OK) == -1) {
		printf("Usage: %s [port] [sock name]\n", *argv);
		return 0;
	}
	tcp_server(argv[1], 10, handler, argv[2], strlen(argv[2]) + 1);
	return 0;
}
