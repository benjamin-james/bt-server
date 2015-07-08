
CC = cc
CFLAGS = -W -Wall -Werror

all: tls-server btd server client bt-server tcp-server tcp-client gui-server

gui-server: src/gui-server.c input.o uds.o
	$(CC) $(CFLAGS) src/gui-server.c uds.o input.o -o gui-server -lSDL2
tcp-server: src/tcp-server.c uds.o tcp.o
	$(CC) $(CFLAGS) src/tcp-server.c tcp.o uds.o -o tcp-server
tcp-client: src/tcp-client.c uds.o tcp.o
	$(CC) $(CFLAGS) src/tcp-client.c tcp.o uds.o -o tcp-client
bt-server: src/bt-server.c
	$(CC) $(CFLAGS) src/bt-server.c -o bt-server -lbluetooth
tls-server: src/tls-server.c uds.o
	$(CC) src/tls-server.c uds.o -o tls-server -lssl -lcrypto
btd: src/btd.c uds.o
	$(CC) $(CFLAGS) uds.o src/btd.c -o btd -lbluetooth
server: src/server.c uds.o
	$(CC) $(CFLAGS) uds.o src/server.c -o server
client: src/client.c
	$(CC) $(CFLAGS) src/client.c -o client -lreadline
uds.o: src/uds.c
	$(CC) $(CFLAGS) -c src/uds.c -o uds.o
input.o: src/input.c
	$(CC) $(CFLAGS) -c src/input.c -o input.o
tcp.o: src/tcp.c
	$(CC) $(CFLAGS) -c src/tcp.c -o tcp.o
clean:
	rm *.o
	rm btd
	rm server
	rm client
	rm tls-server
	rm bt-server
	rm gui-server
	rm tcp-server
	rm tcp-client
