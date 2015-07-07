#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "uds.h"
#define SOCK_NAME "./demo_socket"
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
	return uds_server(SOCK_NAME, connection_handler, NULL, 0);
}
