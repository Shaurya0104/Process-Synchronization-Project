#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>

#define MAX_CARS 10 // maximum number of cars on the bridge at once
#define MAX_WEIGHT 1000 // maximum weight capacity of the bridge
#define NUM_CARS 20 // total number of cars to cross the bridge

// global variables
int num_cars_on_bridge = 0; // number of cars currently on the bridge
int total_weight_on_bridge = 0; // total weight of cars currently on the bridge
sem_t sem_bridge; // semaphore for accessing the bridge

// function for a car to cross the bridge
void* cross_bridge(void* car_num) {
    int car_id = *(int*)car_num;
    int car_weight = rand() % 500 + 500; // generate a random weight for the car
    
    // wait for the bridge to be available
    sem_wait(&sem_bridge);
    
    // check if the bridge can support the weight of the car
    while (total_weight_on_bridge + car_weight > MAX_WEIGHT) {
        printf("Car %d is waiting to cross the bridge.\n", car_id);
        sem_post(&sem_bridge);
        sleep(1); // wait for a short time before trying again
        sem_wait(&sem_bridge);
    }
    
    // cross the bridge
    printf("Car %d is crossing the bridge.\n", car_id);
    num_cars_on_bridge++;
    total_weight_on_bridge += car_weight;
    sleep(2); // simulate crossing time
    
    // leave the bridge
    printf("Car %d has left the bridge.\n", car_id);
    num_cars_on_bridge--;
    total_weight_on_bridge -= car_weight;
    sem_post(&sem_bridge);
    
    // exit the thread
    pthread_exit(NULL);
}

int main() {
    // initialize the semaphore
    sem_init(&sem_bridge, 0, MAX_CARS);
    
    // create an array of car IDs
    int car_ids[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++) {
        car_ids[i] = i + 1;
    }
    
    // create threads for each car
    pthread_t threads[NUM_CARS];
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_create(&threads[i], NULL, cross_bridge, &car_ids[i]);
    }
    
    // join threads
    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // destroy the semaphore
    sem_destroy(&sem_bridge);
    
    return 0;
}
