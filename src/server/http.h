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

  typedef struct {

    char *path;
    char *version;
    HttpMethod method;
    HashTable *path_data;
    HashTable *query;
    HashTable *body;
    HashTable *headers;

  } Request;

  typedef struct {

  } Response;

  HttpMethod get_http_method(char *method_str);
  char *get_http_method_str(HttpMethod method);
  Request *initialize_request(char *request_str);

#endif
