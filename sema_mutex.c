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
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

static volatile int shared_value = 2;
pthread_mutex_t process_mutex;

void print_current_time() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%H:%M:%S", timeinfo);
    printf("Current time: %s\n", buffer);
}

void *thread1(void *arg) {
    printf("%s: begin\n", (char *)arg);
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&process_mutex);
        print_current_time();
        shared_value += 10;
        printf("%s: Value after adding 10: %d\n", (char *)arg, shared_value);
        pthread_mutex_unlock(&process_mutex);
        usleep(rand() % 9001 + 1000);
    }
    printf("%s: done\n", (char *)arg);
    return NULL;
}

void *thread2(void *arg) {
    printf("%s: begin\n", (char *)arg);
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&process_mutex);
        print_current_time();
        shared_value *= 10;
        printf("%s: Value after multiplying by 10: %d\n", (char *)arg, shared_value);
        pthread_mutex_unlock(&process_mutex);
        usleep(rand() % 9001 + 1000);
    }
    printf("%s: done\n", (char *)arg);
    return NULL;
}

void *thread3(void *arg) {
    printf("%s: begin\n", (char *)arg);
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&process_mutex);
        print_current_time();
        shared_value -= 1;
        printf("%s: Value after subtracting 1: %d\n", (char *)arg, shared_value);
        pthread_mutex_unlock(&process_mutex);
        usleep(rand() % 9001 + 1000);
    }
    printf("%s: done\n", (char *)arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    pthread_mutex_init(&process_mutex, NULL);

    pthread_t p1, p2, p3;
    printf("main: begin (shared_value = %d)\n", shared_value);

    pthread_create(&p1, NULL, thread1, (void *)"A");
    pthread_create(&p2, NULL, thread2, (void *)"B");
    pthread_create(&p3, NULL, thread3, (void *)"C");

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    printf("main: done with all (shared_value = %d)\n", shared_value);

    pthread_mutex_destroy(&process_mutex);
    return 0;
}