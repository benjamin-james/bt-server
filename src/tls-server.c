#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4321
int bt_connect(const char *bt_addr);
int create_socket(int port);
void init_openssl(void);
void cleanup_openssl();
void create_context(SSL_CTX **ctx);
void configure_context(SSL_CTX *ctx);

int main(int argc, char **argv)
{
	int sock;
	SSL_CTX *ctx;

	init_openssl();
	create_context(&ctx);
	configure_context(ctx);
	sock = create_socket(PORT);
	while (1) {
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		SSL *ssl;
		const char *reply = "test\n";
		int client = accept(sock, (struct sockaddr *)&addr, &len);
		if (client < 0) {
			perror("could not accept client\n");
			exit(EXIT_FAILURE);
		}
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		if (SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
		} else {
			SSL_write(ssl, reply, strlen(reply));
			int btsock = bt_connect("01:23:45:67:89:AB");
			close(btsock);
		}
		SSL_free(ssl);
		close(client);
	}
	close(sock);
	SSL_CTX_free(ctx);
	EVP_cleanup();
	return 0;
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
