#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "rwlock.h"

#define MAX_READER      4
#define MAX_WRITER      2
#define MAX_THREADS     (MAX_READER + MAX_WRITER)

void random_sleep()
{
    sleep(rand() % 10);
}

void *reader(void *arg)
{
    rwlock_t *lock = (rwlock_t *)arg;
    pthread_t tid = pthread_self();

    rwlock_read_lock(lock);

    printf("Reader thread (ID: 0x%lx): reading...\n", (unsigned long)tid);
    random_sleep();

    rwlock_read_unlock(lock);

    return NULL;
}

void *writer(void *arg)
{
    rwlock_t *lock = (rwlock_t *)arg;
    pthread_t tid = pthread_self();

    rwlock_write_lock(lock);
    printf("Writer thread (ID: 0x%lx): writing...\n", (unsigned long)tid);
    random_sleep();
    rwlock_write_unlock(lock);

    return NULL;
}

int main(int argc, char **argv)
{
    rwlock_t lock;
    rwlock_init(&lock);

    pthread_t threads[MAX_THREADS];

    srand(time(NULL));

    for (int i = 0; i < MAX_READER; i++)
        pthread_create(&threads[i], NULL, reader, &lock);

    for (int i = 0; i < MAX_WRITER; i++)
        pthread_create(&threads[MAX_READER + i], NULL, writer, &lock);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
