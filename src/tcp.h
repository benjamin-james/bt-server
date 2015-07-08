#ifndef TCP_H
#define TCP_H

#include <stddef.h>

void tcp_server(const char *port, int backlog, void (*func)(int sock, void *data, size_t size), void *data, size_t size);
int tcp_client(const char *hostname, const char *port);

#endif
