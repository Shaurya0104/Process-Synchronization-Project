#include <pthread.h>
#include <iostream>
#include<unistd.h>

using namespace std;

pthread_mutex_t mutex_resource = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_readcount = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_writer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_reader = PTHREAD_COND_INITIALIZER;

int resource = 0;
int readcount = 0;
int writer_waiting = 0;
int writer_active = 0;

void *reader(void *arg) {
    int id = *((int *) arg);
    while (1) {
        pthread_mutex_lock(&mutex_readcount);
        while (writer_waiting || writer_active) {
            pthread_cond_wait(&cond_reader, &mutex_readcount);
        }
        readcount++;
        if (readcount == 1) {
            pthread_mutex_lock(&mutex_resource);
        }
        pthread_mutex_unlock(&mutex_readcount);
        cout << "Reader " << id << " read resource " << resource << endl;
        pthread_mutex_lock(&mutex_readcount);
        readcount--;
        if (readcount == 0) {
            pthread_mutex_unlock(&mutex_resource);
        }
        pthread_mutex_unlock(&mutex_readcount);
        pthread_cond_signal(&cond_writer);
        sleep(1);
    }
}

void *writer(void *arg) {
    int id = *((int *) arg);
    while (1) {
        pthread_mutex_lock(&mutex_readcount);
        writer_waiting++;
        while (readcount > 0 || writer_active) {
            pthread_cond_wait(&cond_writer, &mutex_readcount);
        }
        writer_waiting--;
        writer_active = 1;
        pthread_mutex_unlock(&mutex_readcount);

        cout << "Writer " << id << " is writing resource" << endl;
        resource++;

        pthread_mutex_lock(&mutex_readcount);
        writer_active = 0;
        if (writer_waiting > 0) {
            pthread_cond_signal(&cond_writer);
        } else {
            pthread_cond_broadcast(&cond_reader);
        }
        pthread_mutex_unlock(&mutex_readcount);
        sleep(1);
    }
}

int main() {
    int i;
    pthread_t readers[5], writers[5];
    int reader_ids[5], writer_ids[5];

    for (i = 0; i < 5; i++) {
        reader_ids[i] = i+1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    for (i = 0; i < 5; i++) {
        writer_ids[i] = i+1;
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);
    }

    for (i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(writers[i], NULL);
    }

    return 0;
}
