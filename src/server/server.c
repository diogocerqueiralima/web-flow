#include "server.h"
#include <stdlib.h>

HttpServer *httpServer = NULL;

void initialize_http_server(int port) {

  if (httpServer != NULL) {
    throw_system_error(WARN, "HTTP Server is already initialized");
    return;
  }

  httpServer = malloc(sizeof(HttpServer));

  if (httpServer == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  httpServer->starting = 1;

  int socketFD = tcp_socket_server_init(port);

  if (socketFD < 0)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  ThreadPool *threadPool = create_thread_pool(DEFAULT_THREAD_POOL_SIZE);

  if (threadPool == NULL)
    throw_system_error(FATAL, "It was not possible initialize the http server");

  httpServer->starting = 0;
  httpServer->closing = 0;
  httpServer->port = port;
  httpServer->socketFD = socketFD;
  httpServer->threadPool = threadPool;

  signal(SIGINT, shutdown_http_server);
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
