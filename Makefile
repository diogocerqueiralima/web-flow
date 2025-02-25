CC = gcc

CFLAGS += -Wall
CFLAGS += -g

BINS = main

all: $(BINS)

main: main.c sockets.o errors.o threads.o
	$(CC) $(CFLAGS) -o main main.c sockets.o errors.o threads.o

sockets.o: sockets/sockets.c sockets/sockets.h errors/errors.h
	$(CC) $(CFLAGS) -c sockets/sockets.c -o sockets.o

threads.o: threads/threads.c threads/threads.h errors/errors.h
	$(CC) $(CFLAGS) -c threads/threads.c -o threads.o

errors.o: errors/errors.c errors/errors.h
	$(CC) $(CFLAGS) -c errors/errors.c -o errors.o

clean:
	$(RM) $(BINS) *.o
