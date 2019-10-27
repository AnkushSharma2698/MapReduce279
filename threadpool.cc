#include "threadpool.hpp"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

/**
* A C style constructor for creating a new ThreadPool object
* Parameters:
*     num - The number of threads to create
* Return:
*     ThreadPool_t* - The pointer to the newly created ThreadPool object
*/
typedef void* (*temp)(void *);
void ThreadPool_create(ThreadPool_t &pool, int num) { // Expects an empty pool
    // Create the thread pool of size num
    // The pool should also have locks for the
    pool.threads.resize(num);
    // Run the threads to do the cool stuff
    for (int i = 0; i < num; i++) {
        if (pthread_create( &pool.threads[i], NULL, [](void * tp) -> void * {
            Thread_run((ThreadPool_t *)tp);
            return NULL;
            }, &pool) != 0) {
            ThreadPool_destroy(&pool);
        }
    }
}

void ThreadPool_destroy(ThreadPool_t *tp) {
    pthread_mutex_destroy(&(tp->mutex));
    pthread_cond_destroy(&tp->notify);
}

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
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg) {

    ThreadPool_args * args = (ThreadPool_args *) arg;
    ThreadPool_work_t work_item;
    work_item.func = func;
    work_item.arg = *args;
    tp->queue.max_heap.push(work_item);
}

/**
* Get a task from the given ThreadPool object
* Parameters:
*     tp - The ThreadPool object being passed
* Return:
*     ThreadPool_work_t* - The next task to run
*/
void * ThreadPool_get_work(ThreadPool_t *tp) {
    return NULL;
}

/**
* Run the next task from the task queue
* Parameters:
*     tp - The ThreadPool Object this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp) {
    // Running the thread loop here
    while (true) {
        pthread_mutex_lock(&(tp->mutex));

        // Wait on the condition variable
        while(!tp->no_task_remaining) {
            pthread_cond_wait(&(tp->notify), &(tp->mutex));
        }

        // break condition
        if (tp->queue.max_heap.empty() && tp->no_task_remaining) {
            break;
        }

        // Grab the task 
        thread_func_t function = tp->queue.max_heap.top().func;
        ThreadPool_args args = tp->queue.max_heap.top().arg;
        tp->queue.max_heap.pop(); // Returns void
        pthread_mutex_unlock(&(tp->mutex));
        function(args.filename);
    }
    pthread_mutex_unlock(&(tp->mutex));
    pthread_exit(NULL);

    return NULL;
}