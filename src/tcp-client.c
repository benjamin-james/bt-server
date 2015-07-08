#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcp.h"
#include "uds.h"

#define SOCK_NAME "./sock-tcp-client"

void sighandler(void)
{
	unlink(SOCK_NAME);
	exit(EXIT_SUCCESS);
}
void connection_handler(int udpsock, void *data, size_t size)
{
	int sock;
	char *ptr, buffer[256];
	ssize_t len;
	memcpy(buffer, data, size);
	ptr = strchr(buffer, ':');
	*ptr++ = '\0';
	sock = tcp_client(buffer, ptr);
	while ((len = recv(udpsock, buffer, sizeof(buffer), 0)) > 0) {
		send(sock, buffer, len, 0);
	}
	close(sock);
	close(udpsock);
}
int main(int argc, char **argv)
{
	if (argc < 2 || strchr(argv[1], ':') == NULL) {
		printf("Usage: %s [hostname]:[port]\n", *argv);
		return 0;
	}
	signal(SIGINT, (void *)sighandler);
	return uds_server(SOCK_NAME, connection_handler, argv[1], strlen(argv[1]) + 1);
}
