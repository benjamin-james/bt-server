#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SOCK_NAME "./demo_socket"
int connection_handler(int connection_fd)
{
	int nbytes = 1;
	char buffer[256];
	while (nbytes > 0) {
		nbytes = read(connection_fd, buffer, 256);
		buffer[nbytes] = 0;
		if (!strcmp(buffer, "exit"))
			break;
		printf("%s\n", buffer);
		nbytes = snprintf(buffer, 256, "%s", buffer);
		write(connection_fd, buffer, nbytes);
	}
	close(connection_fd);
	return 0;
}
int main(void)
{
	struct sockaddr_un address;
	int socket_fd, connection_fd;
	socklen_t address_length;
	pid_t child;
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		printf("socket() failed\n");
		return 1;
	}

	unlink(SOCK_NAME);

	/* start with a clean address structure */
	memset(&address, 0, sizeof(struct sockaddr_un));

	address.sun_family = AF_UNIX;
	sprintf(address.sun_path, SOCK_NAME);

	if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
		printf("bind() failed\n");
		return 1;
	}

	if (listen(socket_fd, 5) != 0) {
		printf("listen() failed\n");
		return 1;
	}

	while ((connection_fd = accept(socket_fd, (struct sockaddr *) &address, &address_length)) > -1) {
		child = fork();
		if(child == 0) {
			/* now inside newly created connection handling process */
			return connection_handler(connection_fd);
		}

		/* still inside server process */
		close(connection_fd);
	}

	close(socket_fd);
	unlink(SOCK_NAME);
	return 0;
}
