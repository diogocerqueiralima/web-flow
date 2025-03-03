#ifndef ROUTES_H

  #define ROUTES_H
  #define DEFAULT_ROUTER_CAPACITY 16

  #include "http.h"
  #include "../utils/hashtable.h"
  #include <stdlib.h>

  typedef struct {

    char *uri;
    HashTable *path;
    HashTable *query;
    HashTable *body;

  } Request;

  typedef struct {

  } Response;

  typedef struct {

    char *path;
    HttpMethod method;
    void (*handler)(Request *, Response *);

  } Route;

  typedef struct {

    Route **routes;
    int size;
    int capacity;

  } Router;

  Router *initialize_router(int capacity);
  int destroy_router(Router *router);
  int register_route(Router *router, char *path, HttpMethod method, void (*handler)(Request *, Response *));

#endif
