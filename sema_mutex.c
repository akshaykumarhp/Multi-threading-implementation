/************************************************************************

Author: Joe Waclawski
		
History    Date         Author           Revision     
           11/8/2023    Joe Waclawski    Initial

This is a program that demonstrates how important protecting a 
shared resource between multiple threads. If this program is run without
any parameter, 2 threads will run, each accessing the shared counter variable.
The counter value in the end should be 20000000 (each thread should 
increment the count 10000000 times). But there is no protection of the 
counter variable, so the actual count is variable (i.e. != 20000000)

If this program is run with the single parameter "y", a binary semaphore will be used to 
protect counter, and the resulting value will always be 20000000
		
Parameters: 1 - "y" to use a mutex; otherwise, no mutex is used.

************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     
#include <string.h>
#include <sys/time.h>   
#include <time.h>      

pthread_mutex_t process_mutex;
long long shared_value = 2; 
int useMutex = 0;

// Function to get current time as a formatted string
void get_current_time(char *buffer, size_t buffer_size) {
    struct timeval tv;
    struct tm *tm_info;
    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);
    // Append microseconds
    size_t len = strlen(buffer);
    snprintf(buffer + len, buffer_size - len, ".%06d", tv.tv_usec);
}

// Thread 1: Adds 10 to shared_value
void *thread1_func(void *arg) {
    unsigned long thread_id = (unsigned long)pthread_self();
    unsigned int seed = (unsigned int)(time(NULL) ^ thread_id); 
    for (int i = 0; i < 100; i++) {
        // Sleep for a random duration between 1,000 and 10,000 microseconds
        int sleep_time = (rand_r(&seed) % 9001) + 1000; 
        usleep(sleep_time);    
        if (useMutex) pthread_mutex_lock(&process_mutex);     
        // Get and print current time
        char time_str[64];
        get_current_time(time_str, sizeof(time_str));
        printf("[Thread 1] Time: %s | Shared Value before addition: %lld\n", time_str, shared_value);     
        // Perform addition
        shared_value += 10;
        printf("[Thread 1] Added 10. Shared Value after addition: %lld\n", shared_value);      
        if (useMutex) pthread_mutex_unlock(&process_mutex);
    }
    pthread_exit(NULL);
}

// Thread 2: Multiplies shared_value by 10
void *thread2_func(void *arg) {
    unsigned long thread_id = (unsigned long)pthread_self();
    unsigned int seed = (unsigned int)(time(NULL) ^ thread_id); 
    for (int i = 0; i < 100; i++) {
        // Sleep for a random duration between 1,000 and 10,000 microseconds
        int sleep_time = (rand_r(&seed) % 9001) + 1000; // 1000 to 10000
        usleep(sleep_time); 
        if (useMutex) pthread_mutex_lock(&process_mutex);  
        // Get and print current time
        char time_str[64];
        get_current_time(time_str, sizeof(time_str));
        printf("[Thread 2] Time: %s | Shared Value before multiplication: %lld\n", time_str, shared_value);    
        // Perform multiplication
        shared_value *= 10;
        printf("[Thread 2] Multiplied by 10. Shared Value after multiplication: %lld\n", shared_value);  
        if (useMutex) pthread_mutex_unlock(&process_mutex);
    }
    pthread_exit(NULL);
}

// Thread 3: Subtracts 1 from shared_value
void *thread3_func(void *arg) {
    unsigned long thread_id = (unsigned long)pthread_self();
    unsigned int seed = (unsigned int)(time(NULL) ^ thread_id); 
    for (int i = 0; i < 100; i++) {
        // Sleep for a random duration between 1,000 and 10,000 microseconds
        int sleep_time = (rand_r(&seed) % 9001) + 1000; // 1000 to 10000
        usleep(sleep_time);     
        if (useMutex) pthread_mutex_lock(&process_mutex);      
        // Get and print current time
        char time_str[64];
        get_current_time(time_str, sizeof(time_str));
        printf("[Thread 3] Time: %s | Shared Value before subtraction: %lld\n", time_str, shared_value);        
        // Perform subtraction
        shared_value -= 1;
        printf("[Thread 3] Subtracted 1. Shared Value after subtraction: %lld\n", shared_value);    
        if (useMutex) pthread_mutex_unlock(&process_mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "y") == 0) {
        useMutex = 1;
        // Initialize the mutex
        if (pthread_mutex_init(&process_mutex, NULL) != 0) {
            perror("Failed to initialize mutex");
            exit(EXIT_FAILURE);
        }
    }
    printf("Initial value of shared_value: %lld\n", shared_value);
    pthread_t thread1, thread2, thread3;
    if (pthread_create(&thread1, NULL, thread1_func, NULL) != 0) {
        perror("Failed to create Thread 1");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread2, NULL, thread2_func, NULL) != 0) {
        perror("Failed to create Thread 2");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread3, NULL, thread3_func, NULL) != 0) {
        perror("Failed to create Thread 3");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thread1, NULL) != 0) {
        perror("Failed to join Thread 1");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thread2, NULL) != 0) {
        perror("Failed to join Thread 2");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thread3, NULL) != 0) {
        perror("Failed to join Thread 3");
        exit(EXIT_FAILURE);
    }
    if (useMutex) {
        pthread_mutex_destroy(&process_mutex);
    }
    printf("Final value: %lld\n", shared_value);
    return 0;
}
