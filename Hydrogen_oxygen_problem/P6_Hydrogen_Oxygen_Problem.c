#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{

    int value;             // Value of Semaphore
    pthread_mutex_t mutex; // mutex variable for exclusive access to value of s as well as for condition variable
    pthread_cond_t c;      // condition variable used for block() and wakeup() operation

} Semaphore;

void sem_wait(Semaphore *s) // Wait function to block process
{
    pthread_mutex_lock(&s->mutex);

    while (s->value == 0)
    {
        pthread_cond_wait(&s->c, &s->mutex);
    }
    (s->value)--;
    pthread_mutex_unlock(&s->mutex);
}

void sem_signal(Semaphore *s) // Signal function to wakeup process
{
    pthread_mutex_lock(&s->mutex);
    (s->value)++;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->mutex);
}

Semaphore empty = { // Initialisation of semaphore
    .value = 1,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .c = PTHREAD_COND_INITIALIZER
};

Semaphore full = {
    .value = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .c = PTHREAD_COND_INITIALIZER
};

Semaphore H_queue;      // Queue for waiting H-atoms
Semaphore O_queue;      // Queue for waiting O-atoms
Semaphore WH_mutex;     // To mutually exclude access of waiting_H variable
Semaphore WO_mutex;     // To mutually exclude access of waiting_O variable
int waiting_H;          // Number of waiting H-atoms
int waiting_O;          // Number of waiting O-atoms

void *Hydrogen(void *num)
{

    int id = *(int *)num; // Hydrogen number

    printf("Hydrogen: %d has arrived.\n", id);

    sem_wait(&WH_mutex);
    sem_wait(&WO_mutex);
    if (waiting_H == 0 || waiting_O == 0)
    {
        sem_signal(&WO_mutex);
        waiting_H++;
        sem_signal(&WH_mutex);
        sem_wait(&H_queue);
    }
    else
    {
        sem_signal(&H_queue);
        waiting_H--;
        sem_signal(&WH_mutex);
        sem_signal(&O_queue);
        waiting_O--;
        sem_signal(&WO_mutex);
    }

    printf("Hydrogen: %d has formed a bond.\n", id);
}

void *Oxygen(void *num)
{

    int id = *(int *)num; // Oxygen number

    printf("Oxygen: %d has arrived.\n", id);

    sem_wait(&WO_mutex);
    waiting_O++;
    sem_signal(&WO_mutex);
    sem_wait(&O_queue);

    printf("Oxygen: %d has formed a bond.\n", id);
    printf("A water molecule has formed.\n");
}

int main()
{

    int N = 10;       // Total number of atoms.
    pthread_t tid[N]; // Declaring threads
    int a[N];

    H_queue = full; // Initializing all the semaphores
    O_queue = full;
    WH_mutex = empty;
    WO_mutex = empty;
    waiting_H = 0;
    waiting_O = 0;

    int cur_H = 0; // Index to be assigned to upcoming hydrogen
    int cur_O = 0; // Index to be assigned to upcoming oxygen

    srand((unsigned int)time(NULL)); // Assigning random sequence of hydrogen and oxygen atoms
    for (int i = 0; i < N; i++)
    {
        if (rand() % 3 == 0)
        { // 67% probability of assigning a hydrogen and 33% probability of assgning a oxygen
            cur_O++;
            a[i] = cur_O;
            pthread_create(&tid[i], NULL, Oxygen, (void *)&a[i]); // Initializing an oxygen thread
        }
        else
        {
            cur_H++;
            a[i] = cur_H;
            pthread_create(&tid[i], NULL, Hydrogen, (void *)&a[i]); // Initialiing a hydrogen thread
        }
        sleep(1); // Each atom arrives 1 second after the previous atom.
    }

    for (int i = 0; i < N; i++)
        pthread_join(tid[i], NULL);

    printf("All threads have been executed successfully.\n");

    return 0;
}