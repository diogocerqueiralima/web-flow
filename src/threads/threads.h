#ifndef THREADS_H

  #define THREADS_H
  #define DEFAULT_THREAD_POOL_SIZE 10

  #include "../errors/errors.h"
  #include <pthread.h>
  #include <stdlib.h>

  typedef struct {
  
    pthread_t *threads;
    int close;
    int size;

  } ThreadPool; 

  int create_thread(ThreadPool *threadPool, pthread_t *threadId);
  ThreadPool *create_thread_pool(int size);
  void shutdown_thread_pool(ThreadPool *threadPool);

#endif
