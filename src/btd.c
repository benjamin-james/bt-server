#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "uds.h"

int bt_connect(const char *addr);
void handle_client(int sock, void *data, size_t size);

#define SOCK_NAME "./bluetooth_sock"
int main(void)
{
	return uds_server(SOCK_NAME, handle_client, NULL, 0);
}

void handle_client(int uds, void *data, size_t datasize)
{
	char buffer[256];
	int bts, num = recv(uds, buffer, sizeof(buffer), 0);
	printf("data: %p size: %lu\n", data, datasize);
	buffer[num] = 0;
	bts = bt_connect(buffer);
	if (bts >= 0)
		sprintf(buffer, "connected\n");
	else {
		sprintf(buffer, "error\n");
		close(bts);
		return;
	}
	send(uds, buffer, strlen(buffer), 0);
	while (1) {
		if ((num = recv(uds, buffer, sizeof(buffer), 0)) <= 0) {
			shutdown(bts, SHUT_WR);
			break;
		}
		send(bts, buffer, num, 0);
		if ((num = recv(bts, buffer, sizeof(buffer), 0)) <= 0) {
			shutdown(uds, SHUT_WR);
			break;
		}
		send(uds, buffer, num, 0);
	}
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
