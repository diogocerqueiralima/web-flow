#include "server/server.h"
#include <stdio.h>

#define PORT 8080

int main() {

  initialize_http_server(PORT);
  start_server();

  return 0;
}
