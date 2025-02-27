#ifndef __RWLOCK_H__
#define __RWLOCK_H__

typedef struct {
    unsigned long readers; 
    unsigned long writer;
} rwlock_t;

void rwlock_init(rwlock_t *lock);
void rwlock_read_lock(rwlock_t *lock);
void rwlock_read_unlock(rwlock_t *lock);
void rwlock_write_lock(rwlock_t *lock);
void rwlock_write_unlock(rwlock_t *lock);

#endif
