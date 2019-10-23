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
    ThreadPool_create(reduce_pool, num_reducers);

    // Main Thread will add jobs to the queue for the mappers





}

void MR_Emit(char *key, char *value) {

}

unsigned long MR_Partition(char *key, int num_partitions) {

}

void MR_ProcessPartition(int partition_number) {

}

char *MR_GetNext(char *key, int partition_number) {

}