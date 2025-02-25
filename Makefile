CC = gcc

CFLAGS += -Wall
CFLAGS += -g

BINS = main

all: $(BINS)

main: src/main.c sockets.o errors.o threads.o
	$(CC) $(CFLAGS) -o main src/main.c sockets.o errors.o threads.o

sockets.o: src/sockets/sockets.c src/sockets/sockets.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/sockets/sockets.c -o sockets.o

threads.o: src/threads/threads.c src/threads/threads.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/threads/threads.c -o threads.o

errors.o: src/errors/errors.c src/errors/errors.h
	$(CC) $(CFLAGS) -c src/errors/errors.c -o errors.o

clean:
	$(RM) $(BINS) *.o
