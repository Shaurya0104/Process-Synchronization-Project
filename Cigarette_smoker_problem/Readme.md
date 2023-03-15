# Problem description
The problem involves three smokers who each want to smoke a cigarette, but each lacks one of three ingredients required to make a cigarette. One smoker has paper but no tobacco, another has tobacco but no paper, and the third has matches but no paper. The agent has an infinite supply of all three ingredients, but can only give one ingredient to a smoker at a time. Each smoker is continually making a cigarette and smoking it. The problem is to synchronize the agent and the smokers so that the smokers receive the ingredients they need and are able to smoke without deadlock.
# Implementation
The main function initializes the semaphores and creates four threads - one for each smoker, one for the agent, and one to start the agent's work. Each smoker thread waits for a signal from the agent that the required ingredients are available. When the signal is received, the smoker thread makes a cigarette, smokes it, and then notifies the agent that the ingredients are available again. The agent thread randomly selects two ingredients, puts them on the table, and signals the corresponding smoker thread that the ingredients are available. 

The code uses pthreads for multi-threading and semaphores(code for semaphores is provided on other header file) for synchronization. The program runs for a total of 63 iterations, with each smoker attempting to smoke 21 cigarettes. Once all cigarettes have been smoked, the program exits.
# Usage
The code can be compiled using a C++ compiler and the pthread library. Once compiled, the program can be run from the command line.
# Output
After running the code on cpp compiler, following output is obtained 
![Smokers_output](https://user-images.githubusercontent.com/101387465/225374656-146305dd-a697-444a-b8d2-9a8419ec262f.jpeg)


