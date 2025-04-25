#include "server/http.h"
#include "server/routes.h"
#include "server/server.h"
#include <stdio.h>

#define PORT 8080

void helloHandler(Request *request, Response *response) {
  response->content_type = APPLICATION_JSON;
  response->send(response, "Hello World!");
}

int main() {

  initialize_http_server(PORT);

  HttpServer *server = get_http_server();
  Router *router = server->router;

  register_route(router, "/hello", GET, helloHandler);

  start_server();

  return 0;
}