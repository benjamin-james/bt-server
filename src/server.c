#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "uds.h"
#define SOCK_NAME "./sock-server"

void sighandler(void)
{
	unlink(SOCK_NAME);
	exit(EXIT_SUCCESS);
}
void connection_handler(int sock, void *data, size_t datasize)
{
	char buffer[256];
	ssize_t len;
	printf("data: %p size: %lu\n", data, datasize);
	while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0)
		printf("message: %s\n", buffer);
	close(sock);
	printf("disconnected\n");
}
int main(void)
{
	signal(SIGINT, (void *)sighandler);
	return uds_server(SOCK_NAME, connection_handler, NULL, 0);
}
