CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lws2_32

all: server client

server: server.o thread_module.o
	$(CC) -o server server.o thread_module.o $(LDFLAGS)

client: client.o
	$(CC) -o client client.o $(LDFLAGS)

server.o: server.c thread_module.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

thread_module.o: thread_module.c thread_module.h
	$(CC) $(CFLAGS) -c thread_module.c

clean:
	del *.o server.exe client.exe