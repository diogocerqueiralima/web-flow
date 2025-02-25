#include "sockets/sockets.h"
#include <stdio.h>

#define PORT 8080

int main() {

  int serverSocketFD = tcp_socket_server_init(8080); 
  int clientSocketFD;

  while((clientSocketFD = tcp_socket_server_accept(serverSocketFD)) > 0) {

    const char *body = "<h1>Olá, Mundo!</h1>\n";
    size_t body_length = strlen(body);

    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s", body_length, body);

    write(clientSocketFD, response, strlen(response));    shutdown(clientSocketFD, SHUT_WR);
  }

}
