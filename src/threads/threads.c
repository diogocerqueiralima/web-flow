#include "threads.h"
#include <pthread.h>

void *thread_routine(void *arg) {

  ThreadPool *threadPool = (ThreadPool *) arg;
  
  while (1) {

    int error; 
    if ((error = pthread_mutex_lock(&threadPool->mutex)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "Failed to lock mutex");
      continue;
    }

    while (threadPool->tasks_size == 0 && !threadPool->close) {

      if ((error = pthread_cond_wait(&threadPool->cond, &threadPool->mutex)) != 0) {

        errno = error;
        throw_system_error(CRITICAL, "Failed to wait on condition variable");

        if ((error = pthread_mutex_unlock(&threadPool->mutex)) != 0) {
          errno = error;
          throw_system_error(CRITICAL, "Failed to unlock mutex");
          continue;
        }

        continue;
      }

    }

    if (threadPool->close) {

      if ((error = pthread_mutex_unlock(&threadPool->mutex)) != 0) {
        errno = error;
        throw_system_error(CRITICAL, "Failed to unlock mutex");
        continue;
      }

      break; 
    }

    Task *task = get_task(threadPool);

    if ((error = pthread_mutex_unlock(&threadPool->mutex)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "Failed to unlock mutex");
      continue;
    }

    if (task != NULL) {
      task->function(task->args);
      free(task);
    }

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

  if (pthread_mutex_init(&threadPool->mutex, NULL) != 0) {
    free(threads);
    free(threadPool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  if (pthread_cond_init(&threadPool->cond, NULL) != 0) {
    free(threads);
    free(threadPool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  threadPool->close = 0;
  threadPool->size = size;
  threadPool->threads = threads;
  threadPool->tasks_size = 0;
  threadPool->tasks_capacity = DEFAULT_TASKS_CAPACITY;
  threadPool->tasks = malloc(sizeof(Task *) * threadPool->tasks_capacity);

  if (threadPool->tasks == NULL) {
    free(threads);
    free(threadPool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  for (int i = 0; i < size; i++) {
    
    int error;

    if ((error = create_thread(threadPool, &threadPool->threads[i])) != 0) {
      shutdown_thread_pool(threadPool);
      throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
    }
    
  }

  return threadPool;
}

int submit_task(ThreadPool *threadPool, void (*function)(void **), void **args) {

  if (threadPool == NULL || function == NULL || args == NULL) {
    throw_system_error(CRITICAL, "It was not possible submit the task");
    return -1;
  }

  Task *task = malloc(sizeof(Task));

  if (task == NULL) {
    throw_system_error(CRITICAL, "It was not possible submit the task");
    return -1;
  }

  task->function = function;
  task->args = args;

  if (threadPool->tasks_size >= threadPool->tasks_capacity) {

    Task **newTasks = realloc(threadPool->tasks, threadPool->tasks_capacity * 2);

    if (newTasks == NULL) {
      throw_system_error(CRITICAL, "It was not possible submit the task");
      free(task);
      return -1;
    }

    threadPool->tasks = newTasks;
    threadPool->tasks_capacity *= 2;
  }

  pthread_cond_signal(&threadPool->cond);
  threadPool->tasks[threadPool->tasks_size++] = task;
  return 0;
}

Task *get_task(ThreadPool *threadPool) {

  if (threadPool->tasks_size == 0)
    return NULL;

  Task *task = threadPool->tasks[0];

  for (int i = 0; i < threadPool->tasks_size - 1; i++) {
    threadPool->tasks[i] = threadPool->tasks[i + 1];
  }

  threadPool->tasks_size--;

  return task;
}

void shutdown_thread_pool(ThreadPool *threadPool) {

  if (threadPool == NULL)
    return; 

  threadPool->close = 1;

  pthread_cond_broadcast(&threadPool->cond);

  for (int i = 0; i < threadPool->size; i++) {

    pthread_t thread = threadPool->threads[i];

    int error;

    if ((error = pthread_join(thread, NULL)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "It was not possible wait for the Thread");
    }

  }

  free(threadPool->threads);
  
  for (int i = 0; i < threadPool->tasks_size; i++) {
    free(threadPool->tasks[i]); 
  }

  int error;
  if ((error = pthread_cond_destroy(&threadPool->cond)) != 0) {
    errno = error;
    throw_system_error(CRITICAL, "it was not possible destroy the mutex");
  }

  if ((error = pthread_mutex_destroy(&threadPool->mutex)) != 0) {
    errno = error;
    throw_system_error(CRITICAL, "it was not possible destroy the mutex");
  }

  free(threadPool->tasks);
  free(threadPool);
}
