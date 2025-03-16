#include "http.h"
#include <string.h>

HttpMethod get_http_method(char *method_str) {

  if (strcmp(method_str, "GET") == 0) {
      return GET;
  } else if (strcmp(method_str, "POST") == 0) {
      return POST;
  } else if (strcmp(method_str, "PUT") == 0) {
      return PUT;
  } else if (strcmp(method_str, "DELETE") == 0) {
      return DELETE;
  } else if (strcmp(method_str, "HEAD") == 0) {
      return HEAD;
  } else {
      throw_system_error(CRITICAL, "What HTTP Method is this?");
      return UNKNOWN;
  }

}

char *get_http_method_str(HttpMethod method) {

  switch (method) {
    case GET: return "GET";
    case POST: return "POST";
    case PUT: return "PUT";
    case DELETE: return "DELETE";
    case HEAD: return "HEAD";
    default: return "UNKNOWN";
  }
  
}

void print_characters(const char *str) {
    while (*str) {
        if (*str == '\r') {
            printf("\\r");
        } else if (*str == '\n') {
            printf("\\n");
        } else if (*str == '\t') {
            printf("\\t");
        } else {
            printf("%c", *str);
        }
        str++;
    }
    printf("\n");
}

Request *initialize_request(char *request_str) {

  printf("%s\n", request_str);

  Request *request = malloc(sizeof(Request));

  if (request == NULL) {
    throw_system_error(CRITICAL, "It was not possible parse the HTTP Request");
    return NULL;
  }

  HashTable *headers = initialize_hash_table(DEFAULT_HASH_TABLE_CAPACITY);

  if (headers == NULL) {
    free(request);
    throw_system_error(CRITICAL, "It was not possible parse the HTTP Request");
    return NULL;
  }

  request->headers = headers;

  char *line_ptr;
  HttpMethod method = get_http_method(strtok_r(request_str, SP, &line_ptr));
  
  if (method == UNKNOWN) {
    free(request_str);
    destroy_hash_table(request->headers);
    free(request);
    throw_system_error(CRITICAL, "It was not possible parse the HTTP Request");
    return NULL;
  }

  char *path = strtok_r(NULL, SP, &line_ptr);
  char *version = strtok_r(NULL, CRLF, &line_ptr);

  //check if version is valid
  
  request->method = method;
  request->path = path;
  request->version = version;

  char *line; 

  while ((line = strtok_r(NULL, CRLF, &line_ptr)) != NULL) { 

    char *header_ptr;
    char *headerName = strtok_r(line, ":", &header_ptr);

    if (header_ptr != NULL) {
        while (*header_ptr == ' ') header_ptr++;
    }

    char *headerValue = header_ptr;

    if (add_hash_table_entry(request->headers, headerName, strlen(headerName), headerValue, strlen(headerValue)) != 0) {
      destroy_hash_table(request->headers);
      free(request);
      throw_system_error(CRITICAL, "Error parsing headers");
      return NULL;
    }

    char *n1 = headerValue + strlen(headerValue) + 2;
    if (*n1 == '\r') break; 
  }

  char *body = strtok_r(NULL, CRLF, &line_ptr);

  return request;
}
