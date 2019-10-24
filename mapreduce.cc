#include "mapreduce.hpp"
#include "threadpool.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// function pointer types used by library functions
void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers) {

    // Create the threadpool
    ThreadPool_t map_pool;
    ThreadPool_t reduce_pool;
    ThreadPool_create(map_pool, num_mappers);
    // Main Thread will add jobs to the queue for the mappers
//    for (int i = 0; i < num_files; i++) {
//      In the main thread just loop and create all of the jobs that will be needed
//        ThreadPool_add_work(map_pool, (thread_func_t *) map, )
//    }
    // Wait until all map threads complete & then destroy the map pool


    // Create R reduce threads, each thread processes a given partition
    // ThreadPool_create(reduce_pool, num_reducers);
    // Wait for all reduce threads to complete
    // Destory the reduce pool
}

void MR_Emit(char *key, char *value) {

}

unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0'){
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

void MR_ProcessPartition(int partition_number) {

}

char *MR_GetNext(char *key, int partition_number) {

}