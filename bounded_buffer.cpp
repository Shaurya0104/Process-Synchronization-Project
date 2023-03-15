#include <iostream>
#include <pthread.h>
#include <atomic>


using namespace std;

typedef struct
{
    atomic_bool locked;
} my_mutex_t;

void my_mutex_init(my_mutex_t *mutex)
{
    atomic_init(&mutex->locked, false);
}

void my_mutex_lock(my_mutex_t *mutex)
{
    while (atomic_exchange(&mutex->locked, true))
        ; 
}

void my_mutex_unlock(my_mutex_t *mutex)
{
    atomic_store(&mutex->locked, false);
}

int buffer_size = 1000;
my_mutex_t sema1;// for critical section
my_mutex_t sema2;  // for checking if producer can produce
my_mutex_t sema3;// if there are any product to consume
atomic<int> check1 = 0; // no. of products in the buffer 
atomic<int> check2 = buffer_size; // number of space left in the buffer
int size_ = 0;  // no. of products in the buffer 
void producer()
{
    // sema1++;
    while (sema2.locked)
        ;
    if (++check1 == buffer_size)
    {
        my_mutex_lock(&sema2);
    }
    while (sema1.locked)
        ;
    my_mutex_lock(&sema1);
    cout << "Producer entered the critcal section\n";
    cout << "producer is producing\n";
    size_++;
    check2--;
    printf("number of product left = %d\n\n", size_);

    my_mutex_unlock(&sema3);
    my_mutex_unlock(&sema1);
    // sema3--;
}

void consumer()
{
    while (sema3.locked)
        ;
    if (++check2 == buffer_size)
    {
        my_mutex_lock(&sema3);
    }
    while (sema1.locked)
        ;
    my_mutex_lock(&sema1);
    cout << "Consumer entered the critcal section\n";
    cout << "Consumer is consuming\n";
    // sema2--;
    size_--;  
    check1--;
    // sema3++;
    printf("number of product left = %d\n\n", size_);
    // sema1++;
    my_mutex_unlock(&sema2);
    my_mutex_unlock(&sema1);
}

int main()
{
    int n1 = 5;
    int n2 = 5;

    pthread_t pro[n1], cons[n2];

    my_mutex_init(&sema1);
    my_mutex_init(&sema2);
    my_mutex_init(&sema3);
    my_mutex_lock(&sema3);

    for (int i = 0; i < n1; i++)
    {
        pthread_create(&pro[i], NULL, (void *(*)(void *))producer, NULL);
    }
    for (int i = 0; i < n2; i++)
    {
        pthread_create(&cons[i], NULL, (void *(*)(void *))consumer, NULL);
    }
    for (int i = 0; i < n1; i++)
    {
        pthread_join(pro[i], NULL);
    }
    for (int i = 0; i < n2; i++)
    {
        pthread_join(cons[i], NULL);
    }

    return 0;
}
