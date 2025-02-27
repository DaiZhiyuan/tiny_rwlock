#include "atomic.h"

#define DELAY_COUNT	300

typedef struct {
    unsigned long readers;
    unsigned long writer;
} rwlock_t;

void rwlock_init(rwlock_t *lock) 
{
    lock->readers = 0;
    lock->writer = 0;
}

void rwlock_read_lock(rwlock_t *lock) 
{
    for( ; ; ) {

        // If there is a writer in the critical section, waiting is required.
        if (lock->writer != 0) {
            machine_pause(DELAY_COUNT);
            continue;
        }

        // Atomic: load readers, readers++, store readers
        fetchadd64_acquire(&lock->readers, 1);

        // double check
        if (lock->writer == 0) {
            break;
        }

        // Atomic: load readers, readers--, store readers
        fetchadd64_release(&lock->readers, -1);
    }
}

void rwlock_read_unlock(rwlock_t *lock) 
{
    fetchadd64_release(&lock->readers, -1);
}

void rwlock_write_lock(rwlock_t *lock) 
{
    for ( ; ; ) {

        // Atomic: Compare-and-Swap 
        if (cas64_acquire(&lock->writer, 1, 0) == 0) {

	    // Wait until there are no readers.
            while (lock->readers > 0) {
		machine_pause(DELAY_COUNT);
            }

            break;
        }

	machine_pause(DELAY_COUNT);
    }
}

void rwlock_write_unlock(rwlock_t *lock) 
{
    // Aomic: Compare-and-Swap
    cas64_release(&lock->writer, 0, 1);
}

