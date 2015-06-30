
CC = cc
CFLAGS = -W -Wall -Werror -ansi

all: bt-server btd server client

tls-server: src/tls-server.c uds.o
	$(CC) src/tls-server.c uds.o -o tls-server -lssl
btd: src/btd.c uds.o
	$(CC) $(CFLAGS) uds.o src/btd.c -o btd -lbluetooth
server: src/server.c
	$(CC) $(CFLAGS) src/server.c -o server
client: src/client.c
	$(CC) $(CFLAGS) src/client.c -o client -lreadline
uds.o: src/uds.c
	$(CC) $(CFLAGS) -c src/uds.c -o uds.o
clean:
	rm *.o
	rm btd
	rm server
	rm client
	rm bt-server
