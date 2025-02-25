#include "sockets.h"
#include "../errors/errors.h"

int tcp_socket_server_init(int port) {

    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocketFD < 0) {
      throw_system_error(CRITICAL, "It was not possible open a TCP Socket");
      return -1; 
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(serverSocketFD, (struct sockaddr *) &server_addr, sizeof(server_addr))) < 0) {
      throw_system_error(CRITICAL, "It was not possible assign a IP Address for the socket");
      return -1;
    }

    if ((listen(serverSocketFD, SOCKET_QUEUE)) < 0) {
      throw_system_error(CRITICAL, "Socket can't listen on this port");
      return -1; 
    }

    return serverSocketFD;
}

int tcp_socket_server_accept(int serverSocketFD) {

  struct sockaddr client_addr;
  __u_int dim_client = sizeof(client_addr); 
  int clientSocketFD = accept(serverSocketFD, &client_addr, &dim_client);

  if (clientSocketFD < 0) {
    throw_system_error(CRITICAL, "It was not possible to accept a connection to this socket");
    return -1;
  }

  return clientSocketFD;
}

int write_socket(int socketFD, void *buffer, size_t count) {
 
  char *theBuffer = buffer;
  size_t bytesToWrite = count;

  while (bytesToWrite > 0) {
    
    int bytesWritten = write(socketFD, theBuffer, bytesToWrite);

    if (bytesWritten < 0) {
      throw_system_error(CRITICAL, "It was not possible write to socket");
      return -1;
    }

    bytesToWrite -= bytesWritten;
    theBuffer += bytesWritten;
  }

  return 0;
}

void *read_socket(int socketFD, size_t bufferSize) {

  size_t currentBufferSize = bufferSize;
  char *buffer = malloc(bufferSize + sizeof(int));

  if (buffer == NULL) {
    throw_system_error(CRITICAL, "It was not possible allocate memory");
    return NULL;
  }

  int *totalBytesRead = (int *) buffer;
  *totalBytesRead = 0;
  int bytesRead;

  while ((bytesRead = read(socketFD, buffer + sizeof(int) + *totalBytesRead, bufferSize)) > 0) {

    *totalBytesRead += bytesRead;

    if (currentBufferSize < *totalBytesRead + bufferSize) {

      currentBufferSize *= 2;
      currentBufferSize += sizeof(int);
      char *newBuffer = realloc(buffer, currentBufferSize);

      if (newBuffer == NULL) {
        throw_system_error(CRITICAL, "It was not possible allocate memory");
        free(buffer);
        return NULL;
      }
    
      buffer = newBuffer;
    }

  }

  if (bytesRead < 0) {
    throw_system_error(CRITICAL, "It was not possible read from the socket");
    free(buffer);
    return NULL;
  }

  return buffer;
}
