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

// Vectors destroy themselves so it should not create memory leaks
void ThreadPool_destroy(ThreadPool_t *tp) {
    // Make sure to destroy the locks and the condition variables
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
    return true;
}

/**
* Run the next task from the task queue
* Parameters:
*     tp - The ThreadPool Object this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp) {
    // Running the thread loop here
    while (true) {
        // Critical section of the code
        pthread_mutex_lock(&(tp->mutex));

        //If the threadpool priority queue is empty and there are no remaining
	//tasks to be added to the queue, that means all threadpool work should be complete 
        if (tp->queue.max_heap.empty()) {
            break;
        }

        // Grab the task 
        thread_func_t function = tp->queue.max_heap.top().func; // Get the function (MAP) 
        ThreadPool_args args = tp->queue.max_heap.top().arg; // Args: (size, filname)
        tp->queue.max_heap.pop(); //Removing from the top of the priority queue
        pthread_mutex_unlock(&(tp->mutex)); //Unlocks the critical section so that another thread can now enter and do processing
        pthread_cond_signal(&tp->notify); // Lets the next thread know that it can now access the critical section
        function(args.filename); // Call the function that was specified in the Threadpool_work_t item, in this case Map
    }
    pthread_mutex_unlock(&(tp->mutex)); // Unlock the critical section of the code in case threads are waiting to exit and the current thread broke on the specified condition
    pthread_cond_signal(&tp->notify); // This notify exists so that if num threads > num jobs, it will signal those threads to continue and be able to exit
    pthread_exit(NULL);// Handles cleanup for each thread

    return NULL;
}
