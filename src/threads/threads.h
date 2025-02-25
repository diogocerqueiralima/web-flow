#ifndef THREADS_H

  #define THREADS_H

  #include <pthread.h>
  #include <stdlib.h>

  typedef struct {
  
    pthread_t *threads;
    int size;

  } ThreadPool; 

  int create_thread(pthread_t *threadId);
  ThreadPool *create_thread_pool(int size);
  void thread_pool_shutdown(ThreadPool *threadPool);

#endif
