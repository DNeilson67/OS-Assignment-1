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
    //lock the buffer_index so there is no other changes from other function
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_COUNT; ++i) {
        int number = rand() % (UPPER_NUM - LOWER_NUM + 1) + LOWER_NUM;
        if (buffer_index < BUFFER_SIZE) {
            buffer[buffer_index++] = number;
            FILE *all_file = fopen("all.txt", "a");
            if (all_file != NULL) {
                fprintf(all_file, "%d\n", number);
                fclose(all_file);
            }
        }
        else { 
	printf("buffer index: %d\n i: %d", buffer_index, i);break;}
    }
    pthread_mutex_unlock(&lock); 
    producer_finished = true;
    return NULL;
}

void *customer(void *arg) {
    int parity = *((int *)arg);
    char filename[20];

    //Print file odd/even depends on the parity
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
    return NULL;
}
int main() {
    pthread_t prod_tid, cust1_tid, cust2_tid;
    int cust1_parity = 0; // This represents even, since number % 2 == 0
    int cust2_parity = 1; // This represents odd, since number % 2 == 1
    
    // Create the thread (1 Producer & 2 Consumer)
    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cust1_tid, NULL, customer, &cust1_parity);
    pthread_create(&cust2_tid, NULL, customer, &cust2_parity);

    pthread_join(prod_tid, NULL);
    pthread_join(cust1_tid, NULL);
    pthread_join(cust2_tid, NULL);

    printf("Every thread has finished.\n");

    return 0;
}
