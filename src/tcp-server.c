#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uds.h"

#define MAX_CLIENTS 32

void on_connection(int sock, int udssock)
{
	char buffer[256];
	ssize_t len;
	printf("connected from %d to %d\n", sock, udssock);
	while ((len = recv(sock, buffer, sizeof(buffer), 0)) >= 0) {
		printf("message: %s\n", buffer);
		send(udssock, buffer, len, 0);
	}
	printf("disconnected to %d\n", sock);
	close(udssock);
}
int main(int argc, char **argv)
{
	int list, sock;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;
	/*pid_t pid;*/
	if (argc < 3) {
		printf("Usage: %s [port] [sock name]\n", *argv);
		return 0;
	}
	list = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));
	bind(list, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(list, MAX_CLIENTS);

	while (1) {
		int udssock;
		clilen = sizeof(cliaddr);
		sock = accept(list, (struct sockaddr *)&cliaddr, &clilen);

		/*		if ((pid = fork()) == 0) {
				close(list);*/
		udssock = uds_client_connect(argv[2]);
		on_connection(sock, udssock);
		/*		exit(EXIT_SUCCESS);*/
		/*}*/
		close(sock);
	}
	return 0;
}
