#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <stdbool.h>
#include <iostream>
#include <queue>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*thread_func_t)(void *arg);

typedef struct ThreadPool_args{
    size_t size;
    char * filename;
} ThreadPool_args;

typedef struct ThreadPool_work_t { // This is structure of a task that goes into the PQ
    thread_func_t func;              // The function pointer
    ThreadPool_args arg;                       // The arguments for the function
    bool operator<(const ThreadPool_work_t& other) const { // Comparator
        // Return true if other is less than this
        return arg.size < other.arg.size;
    }
} ThreadPool_work_t;

struct ThreadPool_work_queue_t{  // Priority queue for the tasks
     std::priority_queue<ThreadPool_work_t> max_heap;
} ;

typedef struct ThreadPool_t{ // This holds the idle threads
    std::vector<pthread_t> threads; // Vector of the threads of Either Map or Reduce
    ThreadPool_work_queue_t queue; // Priority Queue of items
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Want to make sure only one lock exists for the whole pool
} ThreadPool_t;

/**
* A C style constructor for creating a new ThreadPool object
* Parameters:
*     num - The number of threads to create
* Return:
*     ThreadPool_t* - The pointer to the newly created ThreadPool object
*/
void ThreadPool_create(ThreadPool_t& pool, int num);

/**
* A C style destructor to destroy a ThreadPool object
* Parameters:
*     tp - The pointer to the ThreadPool object to be destroyed
*/
void ThreadPool_destroy(ThreadPool_t *tp);

/**
* Add a task to the ThreadPool's task queue
* Parameters:
*     tp   - The ThreadPool object to add the task to
*     func - The function pointer that will be called in the thread
*     arg  - The arguments for the function
* Return:
*     true  - If successful
*     false - Otherwise
*/
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg);

/**
* Get a task from the given ThreadPool object
* Parameters:
*     tp - The ThreadPool object being passed
* Return:
*     ThreadPool_work_t* - The next task to run
*/
void * ThreadPool_get_work(ThreadPool_t *tp);

/**
* Run the next task from the task queue
* Parameters:
*     tp - The ThreadPool Object this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp);


#ifdef __cplusplus
}
#endif
#endif
