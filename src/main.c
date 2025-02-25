#include "sockets/sockets.h"
#include "threads/threads.h"
#include <stdio.h>

#define PORT 8080

int main() {

  create_thread_pool(10);

  int serverSocketFD = tcp_socket_server_init(8080); 
  int clientSocketFD;

  while((clientSocketFD = tcp_socket_server_accept(serverSocketFD)) > 0) {

    void *buffer = read_socket(clientSocketFD, 16);
    int bytesRead = *((int *) buffer);
    char *data = (char *) buffer + sizeof(int);

    printf("Bytes read: %d\n", bytesRead);
    printf("Received data: %.*s\n", bytesRead, data);

    free(buffer);
  }

  return 0;
}
