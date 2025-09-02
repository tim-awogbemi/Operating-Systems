/*
 * CP386 Assignment 4 - Question 1
 * File: Q1_file.c
 * Author: Afo Awogbemi
 * Date: July 21, 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_RESOURCES 5
#define NUM_THREADS 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex;

/* decrease available_resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int thread_number, int count) {
    pthread_mutex_lock(&mutex);
    
    if (available_resources < count) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    else {
        available_resources -= count;
        printf("The thread %d has acquired %d resources and %d more resources are available.\n", 
               thread_number, count, available_resources);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

/* increase available resources by count */
int increase_count(int thread_number, int count) {
    pthread_mutex_lock(&mutex);
    
    available_resources += count;
    printf("The thread %d has released %d resources and %d resources are now available.\n", 
           thread_number, count, available_resources);
    
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* thread_function(void* thread_num_ptr) {
    int thread_number = *((int*)thread_num_ptr);
    int count = 1; // Each thread requests EXACTLY 1 resource
    
    // Try to acquire 1 resource
    if (decrease_count(thread_number, count) == -1) {
        printf("Thread %d could not acquire enough resources.\n", thread_number);
    } 
    else {
        // Simulate work by sleeping
        sleep(1);
        
        // Release 1 resource
        increase_count(thread_number, count);
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];
    
    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }
    
    // Create threads (numbered 0-4 as in examples)
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_nums[i])) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Destroy mutex
    pthread_mutex_destroy(&mutex);
    
    printf("All threads have finished execution. Available resources: %d\n", available_resources);
    
    return 0;
}