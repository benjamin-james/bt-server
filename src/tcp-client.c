#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "uds.h"

#define SOCK_NAME "./tcp-client_sock"

int tcp_connect(const char *ip, int port)
{
	struct sockaddr_in servaddr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);
	return connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
}
void connection_handler(int udpsock, void *data, size_t size)
{
	int sock, port;
	char buffer[256];
	char ip[32];
	ssize_t len;
	memcpy(buffer, data, size);
	sscanf(buffer, "%d %s", &port, ip);
	sock = tcp_connect(ip, port);
	printf("connected to %d\n", sock);
	while ((len = recv(udpsock, buffer, sizeof(buffer), 0)) > 0) {
		printf("message: %s\n", buffer);
		send(sock, buffer, len, 0);
	}
	printf("disconnected to %d\n", sock);
	close(sock);
	close(udpsock);
}
int main(int argc, char **argv)
{
	char *ptr;
	char data[256];
	if (argc < 2 || (ptr = strchr(argv[1], ':')) == NULL) {
		printf("Usage: %s [ip]:[port]\n", *argv);
		return 0;
	}
	*ptr++ = '\0';
	sprintf(data, "%s %s", ptr, argv[1]);
	return uds_server(SOCK_NAME, connection_handler, data, strlen(data));
}
