#include "threads.h"
#include <pthread.h>

void *thread_routine(void *arg) {

  ThreadPool *thread_pool = (ThreadPool *) arg;
  
  while (1) {

    int error; 
    if ((error = pthread_mutex_lock(&thread_pool->mutex)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "Failed to lock mutex");
      continue;
    }

    while (thread_pool->tasks_size == 0 && !thread_pool->close) {

      if ((error = pthread_cond_wait(&thread_pool->cond, &thread_pool->mutex)) != 0) {

        errno = error;
        throw_system_error(CRITICAL, "Failed to wait on condition variable");

        if ((error = pthread_mutex_unlock(&thread_pool->mutex)) != 0) {
          errno = error;
          throw_system_error(CRITICAL, "Failed to unlock mutex");
          continue;
        }

        continue;
      }

    }

    if (thread_pool->close) {

      if ((error = pthread_mutex_unlock(&thread_pool->mutex)) != 0) {
        errno = error;
        throw_system_error(CRITICAL, "Failed to unlock mutex");
        continue;
      }

      break; 
    }

    Task *task = get_task(thread_pool);

    if ((error = pthread_mutex_unlock(&thread_pool->mutex)) != 0) {
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
int create_thread(ThreadPool *thread_pool, pthread_t *thread_id) {

  int error = pthread_create(thread_id, NULL, thread_routine, thread_pool);

  if (error != 0) {
    errno = error;
    throw_system_error(CRITICAL, "Failed to create thread");
    return -1;
  }

  return 0;
}

ThreadPool *create_thread_pool(int size) {

  ThreadPool *thread_pool = malloc(sizeof(ThreadPool));

  if (thread_pool == NULL)
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");

  pthread_t *threads = malloc(sizeof(pthread_t) * size);

  if (threads == NULL){
    free(thread_pool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  if (pthread_mutex_init(&thread_pool->mutex, NULL) != 0) {
    free(threads);
    free(thread_pool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  if (pthread_cond_init(&thread_pool->cond, NULL) != 0) {
    free(threads);
    free(thread_pool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  thread_pool->close = 0;
  thread_pool->size = size;
  thread_pool->threads = threads;
  thread_pool->tasks_size = 0;
  thread_pool->tasks_capacity = DEFAULT_TASKS_CAPACITY;
  thread_pool->tasks = malloc(sizeof(Task *) * thread_pool->tasks_capacity);

  if (thread_pool->tasks == NULL) {
    free(threads);
    free(thread_pool);
    throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
  }

  for (int i = 0; i < size; i++) {
    
    int error;

    if ((error = create_thread(thread_pool, &thread_pool->threads[i])) != 0) {
      shutdown_thread_pool(thread_pool);
      throw_system_error(FATAL, "It was not possible initialize the Thread Pool");
    }
    
  }

  return thread_pool;
}

int submit_task(ThreadPool *thread_pool, void (*function)(void **), void **args) {

  if (thread_pool == NULL || function == NULL || args == NULL) {
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

  if (thread_pool->tasks_size >= thread_pool->tasks_capacity) {

    Task **new_tasks = realloc(thread_pool->tasks, thread_pool->tasks_capacity * 2);

    if (new_tasks == NULL) {
      throw_system_error(CRITICAL, "It was not possible submit the task");
      free(task);
      return -1;
    }

    thread_pool->tasks = new_tasks;
    thread_pool->tasks_capacity *= 2;
  }

  pthread_cond_signal(&thread_pool->cond);
  thread_pool->tasks[thread_pool->tasks_size++] = task;
  return 0;
}

Task *get_task(ThreadPool *thread_pool) {

  if (thread_pool->tasks_size == 0)
    return NULL;

  Task *task = thread_pool->tasks[0];

  for (int i = 0; i < thread_pool->tasks_size - 1; i++) {
    thread_pool->tasks[i] = thread_pool->tasks[i + 1];
  }

  thread_pool->tasks_size--;

  return task;
}

void shutdown_thread_pool(ThreadPool *thread_pool) {

  if (thread_pool == NULL)
    return; 

  thread_pool->close = 1;

  pthread_cond_broadcast(&thread_pool->cond);

  for (int i = 0; i < thread_pool->size; i++) {

    pthread_t thread = thread_pool->threads[i];

    int error;

    if ((error = pthread_join(thread, NULL)) != 0) {
      errno = error;
      throw_system_error(CRITICAL, "It was not possible wait for the Thread");
    }

  }

  free(thread_pool->threads);
  
  for (int i = 0; i < thread_pool->tasks_size; i++) {
    free(thread_pool->tasks[i]); 
  }

  int error;
  if ((error = pthread_cond_destroy(&thread_pool->cond)) != 0) {
    errno = error;
    throw_system_error(CRITICAL, "it was not possible destroy the mutex");
  }

  if ((error = pthread_mutex_destroy(&thread_pool->mutex)) != 0) {
    errno = error;
    throw_system_error(CRITICAL, "it was not possible destroy the mutex");
  }

  free(thread_pool->tasks);
  free(thread_pool);
}
