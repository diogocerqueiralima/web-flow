#ifndef HTTP_H

  #define HTTP_H
  #define SP " "
  #define CR "\r"
  #define LF "\n"
  #define CRLF "\r\n"

  #include <string.h>
  #include <stdlib.h>
  #include "../errors/errors.h"
  #include "../utils/hashtable.h"

  typedef enum {

    GET,
    POST,
    DELETE,
    PUT,
    HEAD,
    UNKNOWN

  } HttpMethod;

  typedef enum {

    TEXT_PLAIN,
    APPLICATION_JSON

  } ContentType;

  typedef struct {

    char *path;
    char *version;
    HttpMethod method;
    HashTable *path_data;
    HashTable *query;
    HashTable *body;
    HashTable *headers;

  } Request;

  typedef struct Response {
    
    ContentType content_type;
    void (*send)(struct Response *response, char *content);
    int client_socket_fd;

  } Response;

  HttpMethod get_http_method(char *method_str);
  char *get_http_method_str(HttpMethod method);
  char *get_content_type_str(ContentType content_type);
  Request *initialize_request(char *request_str);
  Response *initialize_response(void (*send)(Response *response, char *content), int client_socket_fd);
  void destroy_request(Request *request);
  void destroy_response(Response *response);

#endif
