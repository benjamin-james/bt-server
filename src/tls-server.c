#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "uds.h"

void client_do(SSL *ssl, const char *sock_name);
int create_socket(int port);
void init_openssl(void);
void cleanup_openssl();
void create_context(SSL_CTX **ctx);
void configure_context(SSL_CTX *ctx);

int main(int argc, char **argv)
{
	int sock, port;
	SSL_CTX *ctx;
	if (argc < 2) {
		printf("Usage: %s [port] [sock name]\n", *argv);
		return 0;
	}
	port = atoi(argv[1]);
	init_openssl();
	create_context(&ctx);
	configure_context(ctx);
	sock = create_socket(port);
	while (1) {
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		SSL *ssl;
		int client = accept(sock, (struct sockaddr *)&addr, &len);
		if (client < 0) {
			perror("could not accept client\n");
			exit(EXIT_FAILURE);
		}
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		if (SSL_accept(ssl) <= 0)
			ERR_print_errors_fp(stderr);
		else
			client_do(ssl, argv[2]);
		SSL_free(ssl);
		close(client);
	}
	close(sock);
	SSL_CTX_free(ctx);
	EVP_cleanup();
	return 0;
}
void client_do(SSL *ssl, const char *sock_name)
{
	char buffer[256];
	int num, sock = uds_client_connect(sock_name);
	while ((num = SSL_read(ssl, buffer, sizeof(buffer)) > 0)) {
		buffer[num] = 0;
		if (!strcmp(buffer, "exit"))
			break;
		write(sock, buffer, num);
		num = read(sock, buffer, sizeof(buffer));
		SSL_write(ssl, buffer, num);
	}
	close(sock);
}
int create_socket(int port)
{
	int s;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Unable to create socket");
		exit(EXIT_FAILURE);
	}

	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("Unable to bind");
		exit(EXIT_FAILURE);
	}

	if (listen(s, 1) < 0) {
		perror("Unable to listen");
		exit(EXIT_FAILURE);
	}

	return s;
}
void init_openssl(void)
{
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}
void create_context(SSL_CTX **ctx)
{
	const SSL_METHOD *method;
	method = SSLv23_server_method();

	*ctx = SSL_CTX_new(method);
	if (!*ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
}
void configure_context(SSL_CTX *ctx)
{
	SSL_CTX_set_ecdh_auto(ctx, 1);

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) < 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) < 0 ) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
}
