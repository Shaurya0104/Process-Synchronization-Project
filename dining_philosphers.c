#include <unistd.h>          //Provides API for POSIX(or UNIX) OS for system calls
#include <stdio.h>
#include <stdlib.h>          //For exit() and rand()
#include <pthread.h>         //For using pThread library
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

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



int n=5;
sem_t S[5];
int phil[5];
void* philo_funct(void* num);


int main(){

    for(int i=0;i<n;i++){
        sem_init(&S[i],0,1);
    }
    pthread_t threads[n];

    for(int i=0;i<n;i++){
        phil[i]=i;
        pthread_create(&threads[i],NULL,philo_funct,&phil[i]);
        printf("Philosopher %d is thinking\n",i+1);
    }
    for(int i=0;i<n;i++){
        pthread_join(threads[i],NULL);
    }

    return 0;
}

void* philo_funct(void* num){
    while(1){
        int* i =num;
        if(*i!=n-1){
            printf("Philosopher %d is hungry\n",(*i)+1);
            sem_wait(&S[(*i)%n]);
            sem_wait(&S[((*i)+1)%n]);
            printf("Philosopher %d is eating\n",(*i)+1);
            sem_post(&S[(*i)%n]);
            sem_post(&S[((*i)+1)%n]);
            printf("Philosopher %d is thinking\n",(*i)+1);
        }
        else{
            printf("Philosopher %d is hungry2\n",(*i)+1);
            sem_wait(&S[((*i)+1)%n]);
            sem_wait(&S[(*i)%n]);
            printf("Philosopher %d is eating\n",(*i)+1);
            sem_post(&S[(*i)%n]);
            sem_post(&S[((*i)+1)%n]);
            printf("Philosopher %d is thinking\n",(*i)+1);
        }
    }
}
