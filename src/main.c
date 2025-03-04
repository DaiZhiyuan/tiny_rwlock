#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "rwlock.h"

#define NUM_READERS 4
#define NUM_WRITERS 2
#define NUM_ITERATIONS 5

rwlock_t rw_lock = {0};

void* reader(void* arg) 
{
    int id = *((int*)arg);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        arm64_read_lock(&rw_lock);
        printf("Reader %d reading resource\n", id);
        sleep(1);
        arm64_read_unlock(&rw_lock);
        sleep(rand() % 2);
    }
    return NULL;
}

void* writer(void* arg)
{
    int id = *((int*)arg);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        arm64_write_lock(&rw_lock); 
        printf("Writer %d writing to resource\n", id);
        sleep(2); 
        arm64_write_unlock(&rw_lock);
        sleep(rand() % 3);
    }
    return NULL;
}

int main(int argc, char **argb) 
{
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int reader_ids[NUM_READERS], writer_ids[NUM_WRITERS];

    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        if (pthread_create(&readers[i], NULL, reader, &reader_ids[i])) {
            perror("Failed to create reader thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        if (pthread_create(&writers[i], NULL, writer, &writer_ids[i])) {
            perror("Failed to create writer thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("Test completed.\n");

    return 0;
}

