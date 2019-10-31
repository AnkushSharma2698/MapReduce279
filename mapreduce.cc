#include "mapreduce.hpp"
#include "threadpool.hpp"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

using namespace std;

unsigned long partitions; // Global variable to store the number of required partitions
std::vector<std::multimap<string, int> > partitionDataStructure; // Data structure that will hold the key value pairs per partition
pthread_mutex_t partition_lock; // Lock for accessing the partition Data structure to ensure there are no parallel writes 
Reducer reducer_func; // Global handle on the reducer function so it doesnt have to be passed in as an argument (Making my  code work with the starter code)

// function pointer types used by library functions
void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers) {

    reducer_func = concate;
    // Create the threadpool with the appropriate data structure needed
    ThreadPool_t map_pool;

    // Specify the number of partitions and update the Global Data structure to conform to this configuration
    partitions = num_reducers;
    partitionDataStructure.resize(partitions);
    
    // Main Thread will add jobs to the queue for the mappers
    for (int i = 0; i < num_files; i++) {
        // Get the size of the file we are looking at
        struct stat st;
        stat(filenames[i], &st);
        // Build the args for the MR
        ThreadPool_args arg;
        arg.filename = filenames[i];
        arg.size = st.st_size;
        // Add work item to the priority queue
        ThreadPool_add_work(&map_pool, (thread_func_t) map, &arg);
    }
    
    // Create the thread pool to process the jobs created by the main thread
    ThreadPool_create(map_pool, num_mappers);

    // Wait for the threads to finish --> dont go past this point until mapping is done
    for (unsigned int i = 0; i < map_pool.threads.size();i++) {
        pthread_join(map_pool.threads[i], NULL);
    }
    
    // Destroy the mutex and the condition variable
    ThreadPool_destroy(&map_pool);
    
    // Create my reducer threads
    vector<pthread_t> reduceThreads;
    reduceThreads.resize(num_reducers);
    
    // Run all of the reducer threads
    for (unsigned int i = 0; i < reduceThreads.size(); i++) {
        pthread_create(&reduceThreads[i], NULL, [](void * i) -> void * {
            MR_ProcessPartition((*(int *) i));
	        delete (int *) i; 
            return NULL;
            }, new int(i));
    } 
   
    // Wait for the reducer threads to finish
    for (int i = 0; i < num_reducers; i++) {
        pthread_join(reduceThreads[i], NULL);
    }
   
    // Destroy the lock for the partition
    pthread_mutex_destroy(&partition_lock);
}

void MR_Emit(char *key, char *value) {
    // get the index of the partition this key,value pair will be assigned to 
    unsigned long partition_index = MR_Partition(key, partitions); // Which partition this key goes into
    string curr_key(key);
    pthread_mutex_lock(&partition_lock); // Lock the paritionDataStructure
    partitionDataStructure[partition_index].insert(multimap<string, int>::value_type(curr_key, (int)* value));
    pthread_mutex_unlock(&partition_lock); // Unlock the paritionDataStructure
}

// This function is a hash function that assigns a key to a partition
unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0'){
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

// Process the partition given a partition number and provide number counts
void MR_ProcessPartition(int partition_number) {
    while(!partitionDataStructure[partition_number].empty()) {
        string curr_key;
        multimap<string, int>::iterator it;
        // Get the first key
        it = partitionDataStructure[partition_number].begin();
        curr_key = it->first;
        char key[curr_key.length() + 1];
        strcpy(key, curr_key.c_str());
        reducer_func(key, partition_number);
    }
}

char *MR_GetNext(char *key, int partition_number) {
    string curr_key(key);
    // Check if the partition is empty
    if (partitionDataStructure[partition_number].empty()) {
        return NULL;
    }
    // Check if the first item is equal to key that we need
    multimap<string, int>::iterator iter;
    iter = partitionDataStructure[partition_number].begin();
    
    if (iter->first.compare(curr_key) != 0) {
        return NULL;
    }
    // Erase the first item
    partitionDataStructure[partition_number].erase(iter);
    // Return the key
    return key;
}
