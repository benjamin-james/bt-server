#ifndef UDS_H
#define UDS_H

#include <sys/un.h>

int uds_server(const char *sock_name, void (*on_connection)(int fd, void *copy_data, size_t copy_size), void *copy_data, size_t copy_size);
int uds_set_addr(struct sockaddr_un *addr, const char *sock_name);
int uds_client_connect(const char *sock_name);
int send_fd(int sock, int fd);
int recv_fd(int sock);
#endif
