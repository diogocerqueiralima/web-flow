#ifndef THREADS_H

  #define THREADS_H
  #define DEFAULT_THREAD_POOL_SIZE 10
  #define DEFAULT_TASKS_CAPACITY 16

  #include "../errors/errors.h"
  #include <pthread.h>
  #include <stdlib.h>

  typedef struct {

    void (*function)(void **args);
    void **args;

  } Task;

  typedef struct {
  
    pthread_t *threads;
    Task **tasks;
    int tasks_size;
    int tasks_capacity;
    int close;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

  } ThreadPool; 

  int create_thread(ThreadPool *threadPool, pthread_t *threadId);
  ThreadPool *create_thread_pool(int size);
  int submit_task(ThreadPool *threadPool, void (*function)(void **args), void **args);
  Task *get_task(ThreadPool *threadPool);
  void shutdown_thread_pool(ThreadPool *threadPool);

#endif
