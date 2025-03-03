#include "server.h"
#include "routes.h"
#include <stdlib.h>
#include <unistd.h>

HttpServer *httpServer = NULL;

void handle_request(void **args) {

  int *clientSocketFD = (int *) args[0];

  write(*clientSocketFD, "Bye!\n", 5);
  close(*clientSocketFD);

  free(clientSocketFD);
  free(args);
}

void handle_client() {

  ThreadPool *threadPool = httpServer->threadPool;

  int clientSocketFD = tcp_socket_server_accept(httpServer->socketFD);
  int *arg0 = malloc(sizeof(int));

  if (arg0 == NULL) {
    close(clientSocketFD);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
    return;
  }

  *arg0 = clientSocketFD;
  void **args = malloc(sizeof(void *));

  if (args == NULL) {
    free(arg0);
    close(clientSocketFD);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
    return;
  }

  args[0] = arg0;

  if (submit_task(threadPool, &handle_request, args) != 0) {
    free(arg0);
    free(args);
    close(clientSocketFD);
    throw_system_error(CRITICAL, "It was not possible communicate with client");
  }

}

void initialize_http_server(int port) {

  signal(SIGINT, shutdown_http_server);

  if (httpServer != NULL) {
    throw_system_error(WARN, "HTTP Server is already initialized");
    return;
  }

  httpServer = malloc(sizeof(HttpServer));
  
  if (httpServer == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  httpServer->starting = 1;
  httpServer->router = initialize_router(DEFAULT_ROUTER_CAPACITY);

  if (httpServer->router == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  int socketFD = tcp_socket_server_init(port);

  if (socketFD < 0) {
    destroy_router(httpServer->router);
    free(httpServer);
    throw_system_error(FATAL, "It was not possible initialize the http server");
  }

  ThreadPool *threadPool = create_thread_pool(DEFAULT_THREAD_POOL_SIZE);

  if (threadPool == NULL) {
    destroy_router(httpServer->router);
    free(httpServer);
    throw_system_error(FATAL, "It was not possible initialize the http server");
  }

  httpServer->closing = 0;
  httpServer->port = port;
  httpServer->socketFD = socketFD;
  httpServer->threadPool = threadPool;
  httpServer->starting = 0;

}

void start_server() {

  if (httpServer == NULL) {
    throw_system_error(WARN, "HTTP Server is not initialized");
    return;
  }

  throw_system_error(INFO, "Running HTTP Server...");

  while (1) {
    handle_client();
  }

}

void shutdown_http_server() {

  if (httpServer == NULL) {
    throw_system_error(WARN, "HTTP Server is not initialized");
    return;
  }

  if (httpServer->starting) {
    throw_system_error(WARN, "HTTP Server is starting");
    return;
  }

  if (httpServer->closing) {
    throw_system_error(WARN, "HTTP Server is already closing");
    return;
  }
  
  httpServer->closing = 1;
  
  if (destroy_router(httpServer->router) != 0)
    throw_system_error(CRITICAL, "It was not possible destroy the router");

  shutdown_thread_pool(httpServer->threadPool);
  close(httpServer->socketFD);
  free(httpServer);
  throw_system_error(INFO, "HTTP Server closed");
  exit(EXIT_SUCCESS);
}

HttpServer *get_http_server() {
  
  if (httpServer == NULL)
    throw_system_error(FATAL, "HTTP Server is not initialized");

  return httpServer;
}
