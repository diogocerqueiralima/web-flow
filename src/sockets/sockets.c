#include "sockets.h"
#include "../errors/errors.h"
#include <asm-generic/errno.h>

int tcp_socket_server_init(int port) {

    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket_fd < 0) {
      throw_system_error(CRITICAL, "It was not possible open a TCP Socket");
      return -1; 
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(server_socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) < 0) {
      throw_system_error(CRITICAL, "It was not possible assign a IP Address for the socket");
      return -1;
    }

    if ((listen(server_socket_fd, SOCKET_QUEUE)) < 0) {
      throw_system_error(CRITICAL, "Socket can't listen on this port");
      return -1; 
    }

    return server_socket_fd;
}

int tcp_socket_server_accept(int server_socket_fd) {

  struct sockaddr client_addr;
  __u_int dim_client = sizeof(client_addr); 
  int client_socket_fd = accept(server_socket_fd, &client_addr, &dim_client);

  if (client_socket_fd < 0) {
    throw_system_error(CRITICAL, "It was not possible to accept a connection to this socket");
    return -1;
  }

  return client_socket_fd;
}

int write_socket(int socket_fd, void *buffer, size_t count) {
 
  char *the_buffer = buffer;
  size_t bytes_to_write = count;

  while (bytes_to_write > 0) {
    
    int bytes_written = write(socket_fd, the_buffer, bytes_to_write);

    if (bytes_written < 0) {
      throw_system_error(CRITICAL, "It was not possible write to socket");
      return -1;
    }

    bytes_to_write -= bytes_written;
    the_buffer += bytes_written;
  }

  return 0;
}

void *read_socket(int socket_fd, size_t buffer_size) {

  size_t current_buffer_size = buffer_size;
  char *buffer = malloc(buffer_size + sizeof(int));

  if (buffer == NULL) {
    throw_system_error(CRITICAL, "It was not possible allocate memory");
    return NULL;
  }

  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  int *total_bytes_read = (int *) buffer;
  *total_bytes_read = 0;
  int bytes_read;

  while ((bytes_read = read(socket_fd, buffer + sizeof(int) + *total_bytes_read, buffer_size)) > 0) {

    *total_bytes_read += bytes_read;

    if (current_buffer_size < *total_bytes_read + buffer_size) {

      current_buffer_size *= 2;
      current_buffer_size += sizeof(int);
      char *new_buffer = realloc(buffer, current_buffer_size);

      if (new_buffer == NULL) {
        throw_system_error(CRITICAL, "It was not possible allocate memory");
        free(buffer);
        return NULL;
      }
    
      buffer = new_buffer;
    }

  }

  /*if (bytes_read < 0) {
    throw_system_error(CRITICAL, "It was not possible read from the socket");
    free(buffer);
    return NULL;
  }*/

  return buffer;
}
