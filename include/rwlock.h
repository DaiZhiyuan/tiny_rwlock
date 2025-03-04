#ifndef __RWLOCK_H__
#define __RWLCOK_H__

#include <stdint.h>

typedef struct { 
    volatile uint32_t lock;
} rwlock_t;

static inline void arm64_read_lock(rwlock_t *rw)
{
    uint32_t tmp, tmp2;

    asm volatile(
    "   sevl\n"
#if !defined(USE_LSE)
    /* LL/SC */
    "1: wfe\n"
    "2: ldaxr   %w0, %2\n"
    "   add %w0, %w0, #1\n"
    "   tbnz    %w0, #31, 1b\n"
    "   stxr    %w1, %w0, %2\n"
    "   cbnz    %w1, 2b\n"
#else
    /* LSE atomics */
    "1: wfe\n"
    "2: ldxr    %w0, %2\n"
    "   adds    %w1, %w0, #1\n"
    "   tbnz    %w1, #31, 1b\n"
    "   casa    %w0, %w1, %2\n"
    "   sbc %w0, %w1, %w0\n"
    "   cbnz    %w0, 2b\n"
#endif
    : "=&r" (tmp), "=&r" (tmp2), "+Q" (rw->lock)
    :
    : "cc", "memory");
}

static inline void arm64_read_unlock(rwlock_t *rw)
{
    uint32_t tmp, tmp2;

    asm volatile(
#if !defined(USE_LSE)
    /* LL/SC */
    "1: ldxr    %w0, %2\n"
    "   sub %w0, %w0, #1\n"
    "   stlxr   %w1, %w0, %2\n"
    "   cbnz    %w1, 1b\n"
#else
    /* LSE atomics */
    "   movn    %w0, #0\n"
    "   staddl  %w0, %2\n"
#endif
    : "=&r" (tmp), "=&r" (tmp2), "+Q" (rw->lock)
    :
    : "memory");
}

static inline void arm64_write_lock(rwlock_t *rw)
{
    uint32_t tmp;

    asm volatile(
#if !defined(USE_LSE)
    /* LL/SC */
    "   sevl\n"
    "1: wfe\n"
    "2: ldaxr   %w0, %1\n"
    "   cbnz    %w0, 1b\n"
    "   stxr    %w0, %w2, %1\n"
    "   cbnz    %w0, 2b\n"
#else
    /* LSE atomics */
    "1: mov %w0, wzr\n"
    "2: casa    %w0, %w2, %1\n"
    "   cbz %w0, 3f\n"
    "   ldxr    %w0, %1\n"
    "   cbz %w0, 2b\n"
    "   wfe\n"
    "   b   1b\n"
    "3:\n"
#endif
    : "=&r" (tmp), "+Q" (rw->lock)
    : "r" (0x80000000)
    : "memory");
}

static inline void arm64_write_unlock(rwlock_t *rw)
{
    asm volatile(
#if !defined(USE_LSE)
    "   stlr    wzr, %0\n"
#else
    "   swpl    wzr, wzr, %0\n"
#endif
    : "=Q" (rw->lock) 
    :
    : "memory");
}

#endif
