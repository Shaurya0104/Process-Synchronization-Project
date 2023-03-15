#include <iostream>
#include <pthread.h>
#include <atomic>
#include <vector>
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

int buffer_size = 100;

vector<int> product_array(buffer_size, 0);

my_mutex_t sema1;
my_mutex_t sema2;
my_mutex_t sema3;
atomic<int> check1 = 0;
atomic<int> check2 = buffer_size;
int size_ = 0;
int left_ = 0;
int right_ = 0;
void producer()
{

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
    cout << "producer is producing\n Products left \n";
    size_++;

    product_array[right_ % buffer_size] = rand();

    for (auto it : product_array)
    {
        if (it != 0)
        {
            cout << it << " ";
        }
    }
    cout << endl;
    right_++;
    right_ = right_ % buffer_size;

    my_mutex_unlock(&sema1);

    my_mutex_unlock(&sema3);
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
    cout << "Consumer is consuming\n Products left \n";

    size_--;
    product_array[left_ % buffer_size] = 0;
    left_++;
    left_ = left_ % buffer_size;
    for (auto it : product_array)
    {
        if (it != 0)
        {
            cout << it << " ";
        }
    }
    cout << endl;

    my_mutex_unlock(&sema1);

    my_mutex_unlock(&sema2);
}

int main()
{
    int n1 = 10;
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
