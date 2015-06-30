#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "uds.h"
#define SOCK_NAME "./demo_socket"
void connection_handler(int sock)
{
	char buffer[256];
	ssize_t len;
	while ((len = recv(sock, buffer, sizeof(buffer), 0)) > 0)
		send(sock, buffer, len, 0);
	close(sock);
}
int main(void)
{
	return uds_server(SOCK_NAME, connection_handler);
}
