#ifndef SOCKETS_H 

  #define SOCKETS_H 
  #include <sys/socket.h>

  int tcp_socket_server_init(int port);
  int tcp_socket_server_accept(int socket);

#endif
