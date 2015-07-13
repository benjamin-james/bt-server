#include <float.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "input.h"
#include "uds.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef rmax
#define rmax(a, b) max(max(a, -a), max(b, -b))
#endif

void print_double(char *buf, double d, char **ret)
{
	memcpy(buf, &d, sizeof(d));
	*ret = buf + sizeof(d);
}
void convert(double x, double y, double *a, double *b)
{
	double r,d;
	if (x*x + y*y <= DBL_EPSILON) {
		*a = 0;
		*b = 0;
		return;
	}
	r = rmax(x, y);
	d = (y*y - x*x) / (y*y + x*x);
	if (y < 0.0) {
		r = -d;
		d = -rmax(x, y);
	}
	if (x >= 0.0) {
		*a = r;
		*b = d;
	} else {
		*a = d;
		*b = r;
	}
}
int print_input(char *buf, struct input *in)
{
	double a, b;
	convert(in->x, in->y, &a, &b);
	return sprintf(buf, "%f %f\n", a, b);
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
