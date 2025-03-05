#ifndef SERVER_H

  #define SERVER_H

  #include <stdlib.h>
  #include <unistd.h>
  #include <signal.h>
  #include "../threads/threads.h"
  #include "../sockets/sockets.h"
  #include "../errors/errors.h"
  #include "routes.h"

  typedef struct {

    int port;
    int socket_fd;
    int closing;
    int starting;
    Router *router;
    ThreadPool *thread_pool;

  } HttpServer;

  void initialize_http_server(int port);
  void start_server();
  void shutdown_http_server();
  HttpServer *get_http_server();

#endif
