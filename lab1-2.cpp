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
sleep(1);
if (pthread_mutex_lock(&mutex) != 0) {
    fprintf(stderr, "Error locking mutex in producerThread\n");
    return nullptr;
}
events[i] = 1;
printf("Event %d Produced.\n", i + 1);
if (pthread_cond_signal(&cond_consumer) != 0) {
fprintf(stderr, "Error signaling condition variable in producerThread\n");
    pthread_mutex_unlock(&mutex);
    return nullptr;
}
if (pthread_cond_wait(&cond_producer, &mutex) != 0) {
fprintf(stderr, "Error waiting on condition variable in producerThread\n");
    pthread_mutex_unlock(&mutex);
    return nullptr;
}
if (pthread_mutex_unlock(&mutex) != 0) {
    fprintf(stderr, "Error unlocking mutex in producerThread\n");
    return nullptr;
}
}
return nullptr;
}

void* consumerThread(void* arg) {
for (int i = 0; i < EVENTSNUM; ++i) {
if (pthread_mutex_lock(&mutex) != 0) {
    fprintf(stderr, "Error locking mutex in consumerThread\n");
    return nullptr;
}
while (!events[i]) {
if (pthread_cond_wait(&cond_consumer, &mutex) != 0) {
    fprintf(stderr, "Error waiting on condition variable in consumerThread\n");
    pthread_mutex_unlock(&mutex);
return nullptr;
}
}
printf("Event %d Consumed.\n\n", i + 1);
events[i] = 0;
if (pthread_cond_signal(&cond_producer) != 0) {
    fprintf(stderr, "Error signaling condition variable in consumerThread\n");
    pthread_mutex_unlock(&mutex);
return nullptr;
}
if (pthread_mutex_unlock(&mutex) != 0) {
    fprintf(stderr, "Error unlocking mutex in consumerThread\n");
    return nullptr;
}
}
return nullptr;
}

int main() {
pthread_t producer, consumer;

if (pthread_create(&producer, NULL, producerThread, NULL) != 0) {
    fprintf(stderr, "Error creating producer thread\n");
    return 1;
}
if (pthread_create(&consumer, NULL, consumerThread, NULL) != 0) {
    fprintf(stderr, "Error creating consumer thread\n");
    return 1;
}

if (pthread_join(consumer, NULL) != 0) {
    fprintf(stderr, "Error joining consumer thread\n");
    return 1;
}
if (pthread_join(producer, NULL) != 0) {
    fprintf(stderr, "Error joining producer thread\n");
    return 1;
}

return 0;
}
