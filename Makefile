CC = gcc

CFLAGS += -Wall
CFLAGS += -g

BINS = main

all: $(BINS)

main: src/main.c server.o sockets.o threads.o errors.o routes.o http.o hashtable.o
	$(CC) $(CFLAGS) -o main src/main.c server.o sockets.o threads.o errors.o routes.o http.o hashtable.o

routes.o: src/server/routes.c src/server/routes.h src/server/http.h
	$(CC) $(CFLAGS) -c src/server/routes.c -o routes.o

http.o: src/server/http.c src/server/http.h src/utils/hashtable.h
	$(CC) $(CFLAGS) -c src/server/http.c -o http.o

server.o: src/server/server.c src/server/server.h src/sockets/sockets.h src/errors/errors.h src/threads/threads.h
	$(CC) $(CFLAGS) -c src/server/server.c -o server.o

sockets.o: src/sockets/sockets.c src/sockets/sockets.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/sockets/sockets.c -o sockets.o

threads.o: src/threads/threads.c src/threads/threads.h src/errors/errors.h
	$(CC) $(CFLAGS) -c src/threads/threads.c -o threads.o

errors.o: src/errors/errors.c src/errors/errors.h
	$(CC) $(CFLAGS) -c src/errors/errors.c -o errors.o

hashtable.o: src/utils/hashtable.c src/utils/hashtable.h
	$(CC) $(CFLAGS) -c src/utils/hashtable.c -o hashtable.o

the_tests: the_tests.o hashtable.o
	$(CC) $(CFLAGS) -o the_tests the_tests.o hashtable.o

the_tests.o: tests/hashtable.c src/utils/hashtable.h
	$(CC) $(CFLAGS) -c tests/hashtable.c -o the_tests.o

run-tests: the_tests
	@echo "Running tests..."
	@valgrind ./the_tests

clean:
	$(RM) $(BINS) *.o the_tests
