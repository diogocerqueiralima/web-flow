#include "server.h"
#include "routes.h"
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>

HttpServer *http_server = NULL;

void sendResponse(Response *response, char *content) {

  printf("%s\n", content);

  char buffer[8192];

  int length = snprintf(buffer, 8192,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: %ld\r\n"
    "\r\n"
    "%s\r\n", strlen(content), content);

  printf("%*.s\n", length, buffer);

  write_socket(response->client_socket_fd, buffer, length);
}

void handle_request(void **args) {

  int *client_socket_fd = (int *) args[0];
  char *content = read_socket(*client_socket_fd, 1024);
  int n_bytes = *(int *) content;
  char *data = content + sizeof(int);
  char *request_str = strndup(data, n_bytes);

  Request *request = initialize_request(request_str);
  Response *response = initialize_response(sendResponse, *client_socket_fd);
  Route *route = get_route(http_server->router, request->path, request->method);

  if (route == NULL) {
    //send bad request
    printf("Bad Request\n");
  }else {
    printf("Handler\n");
    route->handler(request, response);
  }

  printf("Free and close\n");
  close(*client_socket_fd);
  free(request_str);
  free(content);
  free(client_socket_fd);
  free(args);
}

void handle_client() {

  ThreadPool *thread_pool = http_server->thread_pool;

  int client_socket_fd = tcp_socket_server_accept(http_server->socket_fd);
  int *arg0 = malloc(sizeof(int));

  if (arg0 == NULL) {
    close(client_socket_fd);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
    return;
  }

  *arg0 = client_socket_fd;
  void **args = malloc(sizeof(void *));

  if (args == NULL) {
    free(arg0);
    close(client_socket_fd);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
    return;
  }

  args[0] = arg0;

  if (submit_task(thread_pool, &handle_request, args) != 0) {
    free(arg0);
    free(args);
    close(client_socket_fd);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
  }

}

void initialize_http_server(int port) {

  signal(SIGINT, shutdown_http_server);

  if (http_server != NULL) {
    throw_system_error(WARN, "HTTP Server is already initialized");
    return;
  }

  http_server = malloc(sizeof(HttpServer));
  
  if (http_server == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  http_server->starting = 1;
  http_server->router = initialize_router(DEFAULT_ROUTER_CAPACITY);

  if (http_server->router == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  int socket_fd = tcp_socket_server_init(port);

  if (socket_fd < 0) {
    destroy_router(http_server->router);
    free(http_server);
    throw_system_error(FATAL, "It was not possible initialize the http server");
  }

  ThreadPool *thread_pool = create_thread_pool(DEFAULT_THREAD_POOL_SIZE);

  if (thread_pool == NULL) {
    destroy_router(http_server->router);
    free(http_server);
    throw_system_error(FATAL, "It was not possible initialize the http server");
  }

  http_server->closing = 0;
  http_server->port = port;
  http_server->socket_fd = socket_fd;
  http_server->thread_pool = thread_pool;
  http_server->starting = 0;

}

void start_server() {

  if (http_server == NULL) {
    throw_system_error(WARN, "HTTP Server is not initialized");
    return;
  }

  throw_system_error(INFO, "Running HTTP Server...");

  while (1) {
    handle_client();
  }

}

void shutdown_http_server() {

  if (http_server == NULL) {
    throw_system_error(WARN, "HTTP Server is not initialized");
    return;
  }

  if (http_server->starting) {
    throw_system_error(WARN, "HTTP Server is starting");
    return;
  }

  if (http_server->closing) {
    throw_system_error(WARN, "HTTP Server is already closing");
    return;
  }
  
  http_server->closing = 1;
  
  if (destroy_router(http_server->router) != 0)
    throw_system_error(CRITICAL, "It was not possible destroy the router");

  shutdown_thread_pool(http_server->thread_pool);
  close(http_server->socket_fd);
  free(http_server);
  throw_system_error(INFO, "HTTP Server closed");
  exit(EXIT_SUCCESS);
}

HttpServer *get_http_server() {
  
  if (http_server == NULL)
    throw_system_error(FATAL, "HTTP Server is not initialized");

  return http_server;
}
