# Multi-Threaded Bridge Crossing Problem
This program simulates a bridge that can hold a maximum number of cars and a maximum weight capacity. Cars are created as threads, and each car attempts to cross the bridge by acquiring a semaphore that controls access to the bridge. If the bridge is available and can support the weight of the car, the car crosses the bridge and then releases the semaphore. Otherwise, the car waits until the bridge is available.

## Implementation
The program uses the pthread library for multi-threading and semaphores for synchronization. The cross_bridge function simulates a car crossing the bridge. The function generates a random weight for the car and then attempts to acquire the semaphore that controls access to the bridge. If the bridge is not available or cannot support the weight of the car, the car waits for a short time before trying again. Once the car has crossed the bridge, it releases the semaphore.

The main function initializes the semaphore, creates an array of car IDs, and then creates threads for each car. Once all cars have crossed the bridge, the semaphore is destroyed and the program exits.

## Usage
The program can be compiled using a C++ compiler and the pthread library. Once compiled, the program can be run from the command line.

```shell
$ g++ problem_name.cpp -o problem_name -lpthread
$ ./problem_name
```
The program will output information about each car as it crosses the bridge.

## Further Improvements
One way to improve this program would be to add more realistic features, such as traffic lights to control the flow of traffic, or a queue system for cars waiting to cross the bridge. Another improvement would be to add a graphical user interface that displays the current state of the bridge and the location of each car.