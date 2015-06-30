#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include "uds.h"
int uds_server(const char *sock_name, void (*on_connection)(int fd))
{
	struct sockaddr_un addr;
	int sock, conn;
	socklen_t addr_len;
	pid_t child;
	sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket failed");
		return -1;
	}
	unlink(sock_name);
	uds_set_addr(&addr, sock_name);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		perror("bind failed");
		return -1;
	}
	if (listen(sock, 5) != 0) {
		perror("listen failed");
		return -1;
	}
	while ((conn = accept(sock, (struct sockaddr *) &addr, &addr_len)) > -1) {
		child = fork();
		if (child == 0)
			on_connection(conn);
		close(conn);
	}
	close(sock);
	unlink(sock_name);
	return 0;
}
int uds_set_addr(struct sockaddr_un *addr, const char *sock_name)
{
	memset(addr, 0, sizeof(*addr));
	addr->sun_family = AF_UNIX;
	snprintf(addr->sun_path, sizeof(addr->sun_path), "%s", sock_name);
	return 0;
}

int uds_client_connect(const char *sock_name)
{
	struct sockaddr_un addr;
	int sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket failed");
		return -1;
	}
	uds_set_addr(&addr, sock_name);
	if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
		perror("connect failed");
		return -1;
	}
	return sock;
}
