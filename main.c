#include "sockets/sockets.h"

#define PORT 8080

int main() {

  int serverSocketFD = tcp_socket_server_init(8080); 
  int clientSocketFD;

  while((clientSocketFD = tcp_socket_server_accept(serverSocketFD)) > 0) {

    char *buffer = "Hello and bye!\n";
    write(clientSocketFD, buffer, strlen(buffer)); //that is not correct
    close(clientSocketFD);

  }

}
