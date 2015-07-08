#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "input.h"
#include "uds.h"

void print_double(char *buf, double d, char **ret)
{
	memcpy(buf, &d, sizeof(d));
	*ret = buf + sizeof(d);
}
int print_input(char *buf, struct input *in)
{
	char *ptr = buf;
	print_double(ptr, in->x, &ptr);
	print_double(ptr, in->y, &ptr);
	print_double(ptr, in->throttle, &ptr);
	return ptr - buf;
}

int main(int argc, char **argv)
{
	struct input in;
	int sock, len;
	char buffer[256];
	if (argc < 2) {
		printf("Usage: %s [socket name]\n", *argv);
		return 0;
	}
	init(&in, *argv);
	if ((sock = uds_client_connect(argv[1])) < 0) {
		printf("could not create socket\n");
		return -1;
	}
	while (input(&in) == 0) {
		len = print_input(buffer, &in);
		send(sock, buffer, len, 0);
		SDL_Delay(10);
	}
	close(sock);
	return 0;
}
