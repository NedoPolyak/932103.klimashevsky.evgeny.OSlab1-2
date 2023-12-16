#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define EVENTSNUM 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

volatile int events[EVENTSNUM] = { 0 };

void* producerThread(void* arg) {
    for (int i = 0; i < EVENTSNUM; ++i) {
        pthread_mutex_lock(&mutex);
        events[i] = 1;
        printf("Event %d Produced.\n", i + 1);
        pthread_cond_signal(&cond_consumer);
        pthread_cond_wait(&cond_producer, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

void* consumerThread(void* arg) {
    for (int i = 0; i < EVENTSNUM; ++i) {
        pthread_mutex_lock(&mutex);
        while (!events[i]) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        printf("Event %d Consumed.\n\n", i + 1);
        events[i] = 0;
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t producer, consumer;

    pthread_create(&producer, NULL, producerThread, NULL);
    pthread_create(&consumer, NULL, consumerThread, NULL);

    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);

    return 0;
}
