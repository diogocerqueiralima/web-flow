#ifndef HTTP_H

  #define HTTP_H

  #include <string.h>

  typedef enum {

    GET,
    POST,
    DELETE,
    PUT,
    HEAD,
    UNKNOWN

  } HttpMethod;

  HttpMethod get_http_method(char *method_str);

#endif
