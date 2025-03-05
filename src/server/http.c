#include "http.h"
#include "../errors/errors.h"

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

  HttpMethod method = get_http_method(strtok(request_str, SP));
  
  if (method == UNKNOWN) {
    free(request);
    return NULL;
  }

  char *path = strtok(NULL, SP);
  char *version = strtok(NULL, CRLF);

  //check if version is valid
  
  request->method = method;
  request->path = path;
  request->version = version;

  printf("%s\n", get_http_method_str(method));
  printf("%s\n", path);
  printf("%s\n", version);

  return request;
}
