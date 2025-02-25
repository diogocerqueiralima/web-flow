#include "threads.h"
#include "../errors/errors.h"

void *thread_routine(void *arg) {
  printf("running\n");
  return NULL;
}

int create_thread(pthread_t *threadId) {

  int error = pthread_create(threadId, NULL, thread_routine, NULL);

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

  threadPool->size = size;
  threadPool->threads = threads;

  for (int i = 0; i < size; i++) {
    
    int error;

    if ((error = create_thread(&threadPool->threads[i])) != 0) {
      thread_pool_shutdown(threadPool);
      throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
    }
    
  }

  return threadPool;
}

void thread_pool_shutdown(ThreadPool *threadPool) {

  if (threadPool == NULL)
    return; 

  for (int i = 0; i < threadPool->size; i++) {

    pthread_t thread = threadPool->threads[i];

    int error;

    if ((error = pthread_cancel(thread)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "It was not possible cancel the Thread");
    }

    if ((error = pthread_join(thread, NULL)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "It was not possible shutdown the Thread Pool");
    }

  }

  free(threadPool->threads);
  free(threadPool);
}
