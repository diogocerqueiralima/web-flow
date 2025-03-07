#include "http.h"

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

Request *initialize_request(char *request_str) {

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
  char *request_str_copy = strdup(request_str);

  char *line_ptr;
  HttpMethod method = get_http_method(strtok_r(request_str_copy, SP, &line_ptr));
  
  if (method == UNKNOWN) {
    free(request_str_copy);
    destroy_hash_table(request->headers);
    free(request);
    throw_system_error(CRITICAL, "It was not possible parse the HTTP Request");
    return NULL;
  }

  char *path = strtok_r(NULL, SP, &line_ptr);
  char *version = strtok_r(NULL, CRLF, &line_ptr);

  //check if version is valid
  
  request->method = method;
  request->path = strdup(path);
  request->version = strdup(version);

  char *line = strtok_r(NULL, CRLF, &line_ptr); 

  while (line != NULL && strlen(line) > 0) {
  
    char *header_ptr;
    char *headerName = strdup(strtok_r(line, ": ", &header_ptr));
    char *headerValue = strdup(header_ptr);

    printf("%s, %s\n", headerName, headerValue);

    if (add_hash_table_entry(request->headers, headerName, strlen(headerName), headerValue, strlen(headerValue)) != 0) {
      free(request_str_copy);
      destroy_hash_table(request->headers);
      free(request);
      throw_system_error(CRITICAL, "Error parsing headers");
      return NULL;
    }

    line = strtok_r(NULL, CRLF, &line_ptr);
    printf("%s\n", line);
  }

  free(request_str_copy);
  printf("returning...\n");
  return request;
}
