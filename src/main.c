#include "server/server.h"
#include <stdio.h>

#define PORT 8080

int main() {

  initialize_http_server(PORT);
  HttpServer *httpServer = get_http_server(); 

  printf("Socket: %d\n", httpServer->socketFD);

  while (1) {

  }

  return 0;
}
