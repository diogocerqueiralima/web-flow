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
