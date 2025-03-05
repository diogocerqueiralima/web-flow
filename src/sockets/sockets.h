#ifndef SOCKETS_H 

  #define SOCKETS_H 
  #define SOCKET_QUEUE 512

  #include "../errors/errors.h"
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <string.h>
  #include <unistd.h>

  int tcp_socket_server_init(int port);
  int tcp_socket_server_accept(int socket);
  int write_socket(int socket_fd, void *buffer, size_t count);
  void *read_socket(int socket_fd, size_t buffer_size);

#endif
