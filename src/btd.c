#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "uds.h"

int bt_connect(const char *addr);
void handle_client(int sock);

#define SOCK_NAME "./bluetooth_sock"
int main(int argc, char **argv)
{
	return uds_server(SOCK_NAME, handle_client);
}

void handle_client(int uds)
{
	char buffer[256];
	int bts, num = read(uds, buffer, sizeof(buffer));
	buffer[num] = 0;
	bts = bt_connect(buffer);
	if (bts >= 0)
		sprintf(buffer, "connected\n");
	else {
		sprintf(buffer, "error\n");
		close(bts);
		return;
	}
	write(uds, buffer, strlen(buffer));
	while ((num = read(uds, buffer, sizeof(buffer))) > 0) {
		buffer[num] = 0;
		if (!strcmp(buffer, "exit"))
			break;
		write(bts, buffer, num);
		num = read(bts, buffer, sizeof(buffer));
		write(uds, buffer, num);
	}
	close(bts);
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
