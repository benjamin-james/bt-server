#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "uds.h"

int uds_server(const char *sock_name, void (*on_connection)(int fd, void *copy_data, size_t size), void *copy_data, size_t copy_size)
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
		if (child == 0) {
			on_connection(conn, copy_data, copy_size);
			exit(EXIT_SUCCESS);
		}
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
	strcpy(addr->sun_path, sock_name);
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
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		perror("connect failed");
		return -1;
	}
	return sock;
}

/*int recv_fd(int sock)
{
	struct msghdr sock_msg;
	struct iovec io_vector[1];
	struct cmsghdr *control_msg = NULL;
	char msg_buffer[1];
	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
	memset(&sock_msg, 0, sizeof(sock_msg));
	memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));
	io_vector->iov_base = msg_buffer;
	io_vector->iov_len = 1;
	sock_msg.msg_iov = io_vector;
	sock_msg.msg_iovlen = 1;

	sock_msg.msg_control = ancillary_element_buffer;
	sock_msg.msg_controllen = CMSG_SPACE(sizeof(int));

	if (recvmsg(sock, &sock_msg, MSG_CMSG_CLOEXEC) < 0)
		return -1;
	if (*msg_buffer != 'F')
		return -1;
	if ((sock_msg.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
		return -1;
	for (control_msg = CMSG_FIRSTHDR(&sock_msg); control_msg != NULL; control_msg = CMSG_NXTHDR(&sock_msg, control_msg))
		if ((control_msg->cmsg_level == SOL_SOCKET) && (control_msg->cmsg_type == SCM_RIGHTS))
			return *((int *) CMSG_DATA(control_msg));
	return -1;
}

int send_fd(int sock, int fd)
{
	struct msghdr sock_msg;
	struct iovec io_vector[1];
	struct cmsghdr *control_msg = NULL;
	char msg_buffer[1];
	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
	int cmsg_space;
	*msg_buffer = 'F';
	io_vector->iov_base = msg_buffer;
	io_vector->iov_len = 1;
	memset(&sock_msg, 0, sizeof(sock_msg));
	sock_msg.msg_iov = io_vector;
	sock_msg.msg_iovlen = 1;
	cmsg_space = CMSG_SPACE(sizeof(int));
	memset(ancillary_element_buffer, 0, cmsg_space);
	sock_msg.msg_control = ancillary_element_buffer;
	sock_msg.msg_controllen = cmsg_space;

	control_msg = CMSG_FIRSTHDR(&sock_msg);
	control_msg->cmsg_level = SOL_SOCKET;
	control_msg->cmsg_type = SCM_RIGHTS;
	control_msg->cmsg_len = CMSG_LEN(sizeof(int));
	memcpy(CMSG_DATA(control_msg), &fd, sizeof(fd));
	return sendmsg(sock, &sock_msg, 0);
}
*/
