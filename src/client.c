#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv)
{
	struct sockaddr_un addr;
	int  sock, num;
	char *input, buffer[256];

	if (argc < 2) {
		printf("Usage: %s [program]\n", *argv);
		return 0;
	}
	sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket() failed\n");
		return 1;
	}

	/* start with a clean addr structure */
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	sprintf(addr.sun_path, "%s", argv[1]);

	if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
		printf("connect() failed\n");
		return 1;
	}

	rl_bind_key('\t', rl_complete);
	while (1) {
		snprintf(buffer, sizeof(buffer), "> ");
		input = readline(buffer);
		if (!input || !strcmp(input, "exit"))
			break;
		add_history(input);
		send(sock, input, strlen(input), 0);
		free(input);

		num = recv(sock, buffer, sizeof(buffer), 0);
		buffer[num] = 0;
		printf("%s\n", buffer);
	}
	close(sock);
	return 0;
}
