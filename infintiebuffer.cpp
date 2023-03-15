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
        ; // spin until the lock is acquired
}

void my_mutex_unlock(my_mutex_t *mutex)
{
    atomic_store(&mutex->locked, false);
}

my_mutex_t sema1;
my_mutex_t sema2;
int size_ = 0;
void producer()
{

    while (sema1.locked);
    my_mutex_lock(&sema1);

    cout << "Producer entered the critcal section\n";
    cout << "producer is producing\n";
    size_++;

    printf("number of product left = %d\n\n", size_);

    my_mutex_unlock(&sema2);
    my_mutex_unlock(&sema1);
}

void consumer()
{
    while(sema2.locked);


    while (sema1.locked)
        ;
    my_mutex_lock(&sema1);
    
    cout << "Consumer entered the critcal section\n";
    cout << "Consumer is consuming\n";
    // sema2--;
    size_--;
    if (size_ == 0)
    {
        my_mutex_lock(&sema2);
    }
    printf("number of product left = %d\n\n", size_);
    my_mutex_unlock(&sema1);
}

int main()
{
    int n1 = 10;
    int n2 = 10;

    pthread_t pro[n1], cons[n2];

    my_mutex_init(&sema1);
    my_mutex_init(&sema2);
    my_mutex_lock(&sema2);

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
