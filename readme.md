# MapReduce

## By: Ankush Sharma

### Intermediate Key Value Pair

- What I Stored Them In:
    - I created a global `std::vector<std::multimap<string, int>>` to hold the intermediate key value pairs. I used this data structure because it would maintain indexes per partition number. Multimap was used because it contains sorted duplicate key value pairs which was essential for this project.
- How I Store Them:
    - In order to store these key value pairs, I first ran MR_Run which initially builds the priority queue to hold all of the jobs. Following this I create all of the Map threads which run the Thread_run function. Within this method I locked the critical section whenever a thread wants to access the priority queue of jobs, wait if there are still being tasks added to the priority queue, and then I grab a task off the queue and unlock the critical section so the other threads can run. The current thread then runs the thread_func_t of that work task, which in this case is Map. In Map, MR_Emit is called, which takes the current item's key and gives it a partition index using MR_Partition. Following this, in MR_Emit I use a global partition lock to lock a global partition data structure which i then add the key value pair to. This continues until the Priority queue of jobs is empty.

### Time Complexities
- MR_Emit
    - The time complexity of MR_Emit will be O(logn) as the runtime is upper bounded by the insert into a multimap. For n items this will take O(nlogn) time.
- MR_GetNext
    - The time complexity of MR_GetNext will be O(1) as all of the multimap methods for getting and erasing take constant time. For n items this will take O(n).

### ThreadPool Library

#### Task Queue
- The Data structure I used to implement the the task queue in my threadpool library was a std::priority_queue which stores ThreadPool_work_t objects. It sorts items based on file sizes which is illustrated in the custom struct ThreadPool_args.

### Implementation
- In threadpool create I create the number of threads as specified in ThreadPool_create and the threadpool contains a priority queue to hold all jobs needed. These jobs are sorted by file size. Each threadpool contains a mutex lock to handle accessing the priority queue of the threadpool. The thread locks when trying to get from the queue, and unlocks when it is done getting from the queue. During each iteration in the threadRun loop, the thread continually checks to see if the priority queue is empty. Once it is, the thread breaks out of the loop and unlocks the critical section so the other threads may follow suit. Once threadRun is exited, pthread_join() is used to wait for all threads to complete and after this, the ThreadPoolDestroy method is used to destroy the mutex lock.


### Synchronization Primitives
- pthread_mutex_t was used to ensure that any access to the shared data structures was locked. This was used for both the partition and the priority queue.

### Tests of Correctness
- To test correctness I utilized several print statements to see that files are running in the expected order in accordance with file size
- I used the test cases given on Eclass, and since the output was equivalent I believe my code has achieved adequate correctness.
- Tested results files by adding my own words to sample text files to see if the output would be correct.
- Utilized the extra test cases on eclass given by Jihoon
