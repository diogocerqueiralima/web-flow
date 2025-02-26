#include "threads.h"
#include <unistd.h>

void *thread_routine(void *arg) {

  ThreadPool *threadPool = (ThreadPool *) arg;
  
  while (1) {

    if (threadPool->close)
      break;

    sleep(1);
    printf("Thread running...\n");
  }

  return NULL;
}

int create_thread(ThreadPool *threadPool, pthread_t *threadId) {

  int error = pthread_create(threadId, NULL, thread_routine, threadPool);

  if (error != 0) {
    errno = error;
    throw_system_error(CRITICAL, "Failed to create thread");
    return -1;
  }

  return 0;
}

ThreadPool *create_thread_pool(int size) {

  ThreadPool *threadPool = malloc(sizeof(ThreadPool));

  if (threadPool == NULL)
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");

  pthread_t *threads = malloc(sizeof(pthread_t) * size);

  if (threads == NULL){
    free(threadPool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  threadPool->close = 0;
  threadPool->size = size;
  threadPool->threads = threads;

  for (int i = 0; i < size; i++) {
    
    int error;

    if ((error = create_thread(threadPool, &threadPool->threads[i])) != 0) {
      shutdown_thread_pool(threadPool);
      throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
    }
    
  }

  return threadPool;
}

void shutdown_thread_pool(ThreadPool *threadPool) {

  if (threadPool == NULL)
    return; 

  threadPool->close = 1;

  for (int i = 0; i < threadPool->size; i++) {

    pthread_t thread = threadPool->threads[i];

    int error;

    if ((error = pthread_join(thread, NULL)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "It was not possible wait for the Thread");
    }

    printf("Thread closed\n");
  }

  free(threadPool->threads);
  free(threadPool);
}
