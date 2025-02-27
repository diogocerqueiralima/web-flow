#ifndef ROUTES_H

  #define ROUTES_H

  #include "http.h"

  typedef struct {

    char *path;
    HttpMethod method;

  } Route;

#endif
