#include <stdint.h>
#include <stdbool.h>

#ifndef __ATOMICS_H__
#define __ATOMICS_H__

static inline void machine_pause (int32_t delay)
{
    while(delay > 0) {
#if defined(__x86_64__)
        asm volatile ("pause" : : : "memory" );
#elif defined(__aarch64__)
        asm volatile ("yield" : : : "memory" );
#endif  /* ARCH */
        delay--;
    }
}

static inline unsigned long fetchadd64_acquire (unsigned long *ptr, unsigned long val) 
{
#if defined(__x86_64__) && !defined(USE_BUILTIN)
        asm volatile ("lock xaddq %q0, %1\n"
                      : "+r" (val), "+m" (*(ptr))
                      : : "memory", "cc");
#elif defined(__aarch64__) && !defined(USE_BUILTIN)
#if defined(USE_LSE)
        unsigned long old;

        asm volatile(
        "       ldadda  %[val], %[old], %[ptr]\n"
        : [old] "=&r" (old), [ptr] "+Q" (*(unsigned long *)ptr)
        : [val] "r" (val)
        : );

        val = old;
#else
        unsigned long tmp, old, newval;

        asm volatile(
        "1:     ldaxr   %[old], %[ptr]\n"
        "       add     %[newval], %[old], %[val]\n"
        "       stxr    %w[tmp], %[newval], %[ptr]\n"
        "       cbnz    %w[tmp], 1b\n"
        : [tmp] "=&r" (tmp), [old] "=&r" (old), [newval] "=&r" (newval),
          [ptr] "+Q" (*(unsigned long *)ptr)
        : [val] "Lr" (val)
        : );

        val = old;
#endif
#else
        val = __atomic_fetch_add(ptr, val, __ATOMIC_ACQUIRE);
#endif

        return val;
}


static inline unsigned long fetchadd64_release (unsigned long *ptr, unsigned long val) 
{
#if defined(__x86_64__) && !defined(USE_BUILTIN)
        asm volatile ("lock xaddq %q0, %1\n"
                      : "+r" (val), "+m" (*(ptr))
                      : : "memory", "cc");
#elif defined(__aarch64__) && !defined(USE_BUILTIN)
#if defined(USE_LSE)
        unsigned long old;

        asm volatile(
        "       ldaddl  %[val], %[old], %[ptr]\n"
        : [old] "=&r" (old), [ptr] "+Q" (*(unsigned long *)ptr)
        : [val] "r" (val)
        : );

        val = old;
#else
        unsigned long tmp, old, newval;

        asm volatile(
        "1:     ldxr    %[old], %[ptr]\n"
        "       add     %[newval], %[old], %[val]\n"
        "       stlxr   %w[tmp], %[newval], %[ptr]\n"
        "       cbnz    %w[tmp], 1b\n"
        : [tmp] "=&r" (tmp), [old] "=&r" (old), [newval] "=&r" (newval),
          [ptr] "+Q" (*(unsigned long *)ptr)
        : [val] "Lr" (val)
        : );

#endif
        val = old;
#else
        val = __atomic_fetch_add(ptr, val, __ATOMIC_RELEASE);
#endif

        return val;
}

static inline unsigned long cas64_acquire (unsigned long *ptr, unsigned long val, unsigned long exp) 
{
        unsigned long old;

#if defined(__x86_64__) && !defined(USE_BUILTIN)
        asm volatile ("lock cmpxchgq %2, %1\n"
                      : "=a" (old), "+m" (*(ptr))
                      : "r" (val), "0" (exp)
                      : "memory");
#elif defined(__aarch64__) && !defined(USE_BUILTIN)
#if defined(USE_LSE)
        asm volatile(
        "       mov     %[old], %[exp]\n"
        "       casa    %[old], %[val], %[ptr]\n"
        : [old] "=&r" (old), [ptr] "+Q" (*(unsigned long *)ptr)
        : [exp] "Lr" (exp), [val] "r" (val)
        : );
#else
        unsigned long tmp;

        asm volatile(
        "1:     ldaxr   %[old], %[ptr]\n"
        "       eor     %[tmp], %[old], %[exp]\n"
        "       cbnz    %[tmp], 2f\n"
        "       stxr    %w[tmp], %[val], %[ptr]\n"
        "       cbnz    %w[tmp], 1b\n"
        "2:"
        : [tmp] "=&r" (tmp), [old] "=&r" (old),
          [ptr] "+Q" (*(unsigned long *)ptr)
        : [exp] "Lr" (exp), [val] "r" (val)
        : );
#endif
#else
        old = exp;
        __atomic_compare_exchange_n(ptr, &old, val, true, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE);
#endif

        return old;
}

static inline unsigned long cas64_release (unsigned long *ptr, unsigned long val, unsigned long exp) 
{
        unsigned long old;

#if defined(__x86_64__) && !defined(USE_BUILTIN)
        asm volatile ("lock cmpxchgq %2, %1\n"
                      : "=a" (old), "+m" (*(ptr))
                      : "r" (val), "0" (exp)
                      : "memory");
#elif defined(__aarch64__) && !defined(USE_BUILTIN)
#if defined(USE_LSE)
        asm volatile(
        "       mov     %[old], %[exp]\n"
        "       casl    %[old], %[val], %[ptr]\n"
        : [old] "=&r" (old), [ptr] "+Q" (*(unsigned long *)ptr)
        : [exp] "Lr" (exp), [val] "r" (val)
        : );
#else
        unsigned long tmp;

        asm volatile(
        "1:     ldxr    %[old], %[ptr]\n"
        "       eor     %[tmp], %[old], %[exp]\n"
        "       cbnz    %[tmp], 2f\n"
        "       stlxr   %w[tmp], %[val], %[ptr]\n"
        "       cbnz    %w[tmp], 1b\n"
        "2:"
        : [tmp] "=&r" (tmp), [old] "=&r" (old),
          [ptr] "+Q" (*(unsigned long *)ptr)
        : [exp] "Lr" (exp), [val] "r" (val)
        : );
#endif
#else
        old = exp;
        __atomic_compare_exchange_n(ptr, &old, val, true, __ATOMIC_RELEASE, __ATOMIC_RELEASE);
#endif

        return old;
}

#endif /* __ATOMICS_H__ */
