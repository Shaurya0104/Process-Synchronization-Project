#include <unistd.h>          //Provides API for POSIX(or UNIX) OS for system calls
#include <stdio.h>
#include <stdlib.h>          //For exit() and rand()
#include <pthread.h>         //For using pThread library
//#include <semaphore.h>      //For using semaphore APIs

//Global variables required
#define NUMBER_BARBER 3        //number of barbers
#define MAXIMUM_CUSTOMER 20          //maximum number of customers in a Day
#define MAXIMUM_CHAIRS 10      //number of chairs for waiting in shop
#define CUT_TIME 1           //time required by barber to finish haircutting of one customer

//#include <pthread.h>

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int my_sem_trywait(sem_t *sem);
int my_sem_post(sem_t *sem);

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    if (pthread_mutex_init(&sem->mutex, NULL) != 0) {
        return -1;
    }
    if (pthread_cond_init(&sem->cond, NULL) != 0) {
        pthread_mutex_destroy(&sem->mutex);
        return -1;
    }
    sem->value = value;
    return 0;
}

int sem_destroy(sem_t *sem)
{
    if (pthread_cond_destroy(&sem->cond) != 0) {
        return -1;
    }
    if (pthread_mutex_destroy(&sem->mutex) != 0) {
        return -1;
    }
    return 0;
}

int sem_wait(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    while (sem->value == 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int sem_trywait(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    if (sem->value == 0) {
        pthread_mutex_unlock(&sem->mutex);
        return -1;
    }
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int sem_post(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    sem->value++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}


//Semaphore
sem_t barbers;
sem_t clients;
sem_t mutex;

//Functions
void *thread_Barber(void *ptr);
void *thread_Client(void *ptr);
void wait();

//Variables
int availableSeats = MAXIMUM_CHAIRS;    //Counter for Vacant seats in waiting room
int queChair[MAXIMUM_CHAIRS];             //To exchange pid between customer and barber
int nextClient = 0;                        //Index of next candidate for cutting hair
int sitHereNext = 0;
static int cnt = 0;

int main()
{
    pthread_t barber[NUMBER_BARBER],client[MAXIMUM_CUSTOMER];   // declarations of required threads
    int ret_val=0;

    sem_init(&barbers,0,0);
    sem_init(&clients,0,0);
    sem_init(&mutex,0,1);

    printf("Barber Shop Opens\n");
    for(int x=0;x<NUMBER_BARBER;x++)
    {
        ret_val=pthread_create(&barber[x],NULL,thread_Barber,&x);
        sleep(1);
        if(ret_val!=0)
            perror("No Barber Available at this instant..........\n");
    }

    for(int x=0;x<MAXIMUM_CUSTOMER;x++)
    {
        ret_val=pthread_create(&client[x],NULL,thread_Client,&x);
        wait();
        if(ret_val!=0)
            perror("No clients Yet...\n");
    }
    for(int x=0;x<MAXIMUM_CUSTOMER;x++)
        pthread_join(client[x],NULL);
    printf("Maximum customers reached .....Shop Closes\n");
    exit(EXIT_SUCCESS);
}

void *thread_Client(void *ptr)
{
    int seatToBeTaken, barb;   //modified
    sem_wait(&mutex);
    cnt++;
    printf("Client-%d[Id:%d] enters the shop. ",cnt,pthread_self());
    if(availableSeats > 0)
    {
        --availableSeats;
        printf("Client-%d sits in waiting room.\n",cnt);
        sitHereNext = (++sitHereNext) % MAXIMUM_CHAIRS;
        seatToBeTaken = sitHereNext;
        queChair[seatToBeTaken] = cnt;
        sem_post(&mutex);
        sem_post(&barbers);
        sem_wait(&clients);
        sem_wait(&mutex);
        barb = queChair[seatToBeTaken];
        availableSeats++;
        sem_post(&mutex);

    }
    else
    {
        sem_post(&mutex);
        printf("Client-%d finds no seat and leaves.....\n",cnt);
    }
    pthread_exit(0);
}

void *thread_Barber(void *ptr)
{
    int pos = *(int *)(ptr);      //modified
    int customerToBeAddressed, cust;
    printf("Barber-%d[Id:%d] wakes up and starts cutting. ",pos,pthread_self());
    while(1)            //Infinite loop/
    {
        printf("Barber-%d is idle & Gone To Sleep.\n",pos);
        sem_wait(&barbers);
        sem_wait(&mutex);
        nextClient = (++nextClient) % MAXIMUM_CHAIRS;
        customerToBeAddressed = nextClient;
        cust = queChair[customerToBeAddressed];
        queChair[customerToBeAddressed] = pthread_self();
        sem_post(&mutex);
        sem_post(&clients);

        printf("Barber-%d wakes up and is cutting hair of Client-%d.\n",pos,cust);
        sleep(CUT_TIME);
        printf("Barber-%d completes cutting. ",pos);
    }
}

void wait()     //Generates random number between 50000 to 250000/
{
int xe = rand() % (250000 - 50000 + 1) + 50000;
srand(time(NULL));
usleep(xe);     //usleep halts execution in specified miliseconds
}