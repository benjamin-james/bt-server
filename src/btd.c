#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "uds.h"

int bt_connect(const char *addr);
void handle_client(int sock, void *data, size_t size);

#define SOCK_NAME "./sock-bluetooth"

void sighandler(void)
{
	unlink(SOCK_NAME);
	exit(EXIT_SUCCESS);
}
int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Usage: %s [address]\n", *argv);
		return 0;
	}
	signal(SIGINT, (void *)sighandler);
	return uds_server(SOCK_NAME, handle_client, argv[1], strlen(argv[1]) + 1);
}

void handle_client(int uds, void *data, size_t datasize)
{
	char buffer[256];
	int bts;
	ssize_t num;
	memcpy(buffer, data, datasize);
	bts = bt_connect(buffer);
	if (bts < 0) {
		perror("uh oh");
		close(bts);
		close(uds);
		exit(EXIT_FAILURE);
	}
	while ((num = recv(uds, buffer, sizeof(buffer), 0)) >= 0)
		send(bts, buffer, num, 0);
	close(bts);
	close(uds);
}

int bt_connect(const char *bt_addr)
{
	struct sockaddr_rc addr = { 0 };
	int sock;
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = 1;
	str2ba(bt_addr, &addr.rc_bdaddr);
	return connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}
