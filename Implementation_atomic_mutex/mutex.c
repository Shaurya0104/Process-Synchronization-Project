#include <stdbool.h>
#include <stdatomic.h>

typedef struct {
    atomic_bool locked;
} my_mutex_t;

void my_mutex_init(my_mutex_t* mutex)
{
    atomic_init(&mutex->locked, false);
}

void my_mutex_lock(my_mutex_t* mutex)
{
    while (atomic_exchange(&mutex->locked, true))
        ;  // spin until the lock is acquired
}

void my_mutex_unlock(my_mutex_t* mutex)
{
    atomic_store(&mutex->locked, false);
}