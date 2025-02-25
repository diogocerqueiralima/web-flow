#ifndef SOCKETS_H 

  #define SOCKETS_H 
  #define SOCKET_QUEUE 512

  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <string.h>
  #include <unistd.h>

  int tcp_socket_server_init(int port);
  int tcp_socket_server_accept(int socket);

#endif
