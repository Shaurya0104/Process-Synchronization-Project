#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include<string>
#include<iostream>

using namespace std;

sem_t Agent;
sem_t Smoker[3];
sem_t Agready;
sem_t Smready;

int G = -1, s = 1;
int wait_count[3];

string Draw[] = { "paper & matches", "paper & tobacco", "tobacco & matches" };
//char Draw[3][17] = { " paper & matches ", " paper & tobacco ", "tobacco & matches" };

bool Done = false;

void *smoker(void *num)
{
    int *S_num = (int *) (num) ;

    for (int i = 0; i < 21; i++)
    {
        usleep( rand() % 50000);
        cout<<"Smoker "<<*S_num<<" waiting for "<<Draw[*S_num-1]<<" \n";
        s++;

        if(s>3) sem_post(&Smready);

        sem_wait(&Smoker[*S_num-1]) ;

        if(G==0)
        {
            usleep( rand() % 50000);
            cout<<"Smoker 1 is making cigarette\n";
            sleep(1);
            cout<<"Smoking....\n";

            wait_count[0]--;
            sem_post(&Agent);
        }

        else if(G==1)
        {
            usleep( rand() % 50000);
            cout<<"Smoker 2 is making cigarette\n";
            sleep(1);
            cout<<"Smoking....\n";

            wait_count[1]--;
            sem_post(&Agent);
        }

        else if(G==2)
        {
            usleep( rand() % 50000);
            cout<<"Smoker 3 is making cigarette\n";
            sleep(1);
            cout<<"Smoking....\n";

            wait_count[2]--;
            sem_post(&Agent);
        }
    
    }

    pthread_exit(NULL);
}

void *agent(void *num)
{
    srand(time(NULL));
    int *S_num = (int *) (num) ;
    sem_wait(&Agready);

    for (int i = 0; i < 63; i++)
    {
        sem_wait(&Agent);

        while(true)
        {

            int random_int = rand()%3 ;
            usleep(rand()%4000);

            if(wait_count[0]==0 && wait_count[1]==0 && wait_count[2]==0)
            {
                Done = true;
                break;
            }
            else if(wait_count[random_int]!=0)
            {
                sleep(1);
                cout<<" Take "<<Draw[random_int]<<endl;
                G = random_int;
                sem_post(&Smoker[random_int]);
                break;
            }
        }

    }
    
}

void *do_some_work( void *num)
{
    sem_wait(&Smready);
    sem_post(&Agready);
}


    
int main(void)
{
    wait_count[0] = 21;
    wait_count[1] = 21;
    wait_count[2] = 21;

    // 1 -> tobacco    2 -> matches    3 -> paper
    sem_init(&Smoker[0],0,0);
    sem_init(&Smoker[1],0,0);
    sem_init(&Smoker[2],0,0);

    sem_init(&Agent,0,1);

    sem_init(&Agready,0,0);
    sem_init(&Smready,0,0);

    pthread_t smoker1, smoker2, smoker3, agent1, start;

    int *f1 = new int[1];
    f1[0] = 1;

    int *f2 = new int[1];
    f2[0] = 2;

    int *f3 = new int[1];
    f3[0] = 3;

    pthread_create(&smoker1, NULL, smoker, f1);
    pthread_create(&smoker2, NULL, smoker, f2);
    pthread_create(&smoker3, NULL, smoker, f3);

    pthread_create(&agent1, NULL, agent, f1);

    pthread_create(&start, NULL, do_some_work, NULL);

    pthread_join(smoker1, NULL);
    pthread_join(smoker2, NULL);
    pthread_join(smoker3, NULL);

    return 0;

}
