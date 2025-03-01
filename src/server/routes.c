#include "routes.h"
#include <string.h>

Router *initialize_router(int capacity) {

  Router *router = malloc(sizeof(Router));

  if (router == NULL)
    return NULL;

  router->size = 0;
  router->capacity = capacity;
  router->routes = malloc(sizeof(Route *) * capacity);

  if (router->routes == NULL) {
    free(router);
    return NULL;
  }

  return router;
}

int destroy_router(Router *router) {

  if (router == NULL)
    return -1;

  for (int i = 0; i < router->size; i++) {

    Route *route = router->routes[i];

    free(route->path);
    free(route);
  }

  free(router->routes);
  free(router);

  return 0;
}

int register_route(Router *router, char *path, HttpMethod method) {

  if (router == NULL || path == NULL)
    return -1;

  Route *route = malloc(sizeof(Route));

  if (route == NULL)
    return -1;

  route->path = strdup(path);

  if (route->path == NULL) {
    free(route);
    return -1;
  }

  route->method = method;

  if (router->size >= router->capacity) {

    Route **newRoutes = realloc(router->routes, sizeof(Route*) * router->capacity * 2);

    if (newRoutes == NULL) {
      free(route->path);
      free(route);
      return -1;
    }

    router->capacity *= 2;
    router->routes = newRoutes;
  }

  router->routes[router->size++] = route;
  return 0;
}
