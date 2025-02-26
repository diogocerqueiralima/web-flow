CC = gcc

CFLAGS += -Wall
CFLAGS += -g

BINS = main

all: $(BINS)

main: src/main.c server.o sockets.o threads.o errors.o
	$(CC) $(CFLAGS) -o main src/main.c server.o sockets.o threads.o errors.o

server.o: src/server/server.c src/server/server.h src/sockets/sockets.h src/errors/errors.h src/threads/threads.h 
	$(CC) $(CFLAGS) -c src/server/server.c -o server.o

sockets.o: src/sockets/sockets.c src/sockets/sockets.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/sockets/sockets.c -o sockets.o

threads.o: src/threads/threads.c src/threads/threads.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/threads/threads.c -o threads.o

errors.o: src/errors/errors.c src/errors/errors.h
	$(CC) $(CFLAGS) -c src/errors/errors.c -o errors.o

clean:
	$(RM) $(BINS) *.o
