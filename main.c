#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define LOWER_NUM 1
#define UPPER_NUM 10000
#define BUFFER_SIZE 100
#define MAX_COUNT 10000

int buffer[BUFFER_SIZE];
int buffer_index = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool producer_finished = false;

void *producer(void *arg) {
    FILE *file = fopen("all.txt", "w");
    int i = 0;
    for (i; i < MAX_COUNT; i++) {
    	// This allows the number to generate number from a range of 1-10000.
        int number = rand() % (UPPER_NUM - LOWER_NUM + 1) + LOWER_NUM;
        while(buffer_index >= BUFFER_SIZE){
       	//Do nothing if buffer_index >= buffer_size since the buffer can only stores a max of 100.
        }
	// Lock the shared data which is buffered_index and also file
        pthread_mutex_lock(&lock);
        buffer[buffer_index++] = number;
        if (file != NULL) {
            fprintf(file, "%d\n", number);  
        }
        // Unlock if done modifying.
        pthread_mutex_unlock(&lock);
    }

    fclose(file);
    //Debug purposes: Checking the memory leaks
    //printf("%d", i);
    // The customer can be ended if the producer is done.
    producer_finished = true;
}


void *customer(void *arg) {
    int parity = *((int *)arg);
    char filename[20];
    
    sprintf(filename, "%s.txt", (parity == 0) ? "even" : "odd");

    while (true) {
        //Lock and Unlock the buffer_index so that the second customer thread can access.
        pthread_mutex_lock(&lock);
        if (buffer_index == 0 && producer_finished) {
            pthread_mutex_unlock(&lock);
            break;
        }
        if (buffer_index > 0) {
            int number = buffer[buffer_index-1];
            if (number % 2 == parity) {
                FILE *file = fopen(filename, "a");
                if (file != NULL) {
                    fprintf(file, "%d\n", number);
                    fclose(file);
                }
                buffer_index--;
            }
        }
        pthread_mutex_unlock(&lock);
    }
    // Debugging Purposes: To check if the customer thread has finished.
    //printf("Customer %s thread has finished.\n", (parity == 0) ? "even" : "odd");
    return NULL;
}

int main() {
    pthread_t prod_tid, cust1_tid, cust2_tid;
    int cust1_parity = 0; // This represents even, since everything % 2 == 0
    int cust2_parity = 1; // This represents odd, since everything % 2 == 1
    
    // Create the thread (1 Producer & 2 Consumer)
    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cust1_tid, NULL, customer, &cust1_parity);
    pthread_create(&cust2_tid, NULL, customer, &cust2_parity);

    pthread_join(prod_tid, NULL);
    pthread_join(cust1_tid, NULL);
    pthread_join(cust2_tid, NULL);

    // Debugging Purposes: to check every thread is finished.
    //printf("All threads have finished.\n"); 
    return 0;
}
