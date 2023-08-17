#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PRODUCER_THREADS 5
#define NUM_CONSUMER_THREADS 5
#define MAX_COUNT 5
#define BUFFER_SIZE 10

#define CONSUMER_SEM_INIT 0
#define PRODUCER_SEM_INIT BUFFER_SIZE

static void* consumer(void* arg);
static void* producer(void* arg);

static sem_t consumer_semaphore;
static sem_t producer_semaphore;
static sem_t mutex;

static int buffer[BUFFER_SIZE];
static int current_index = -1;

int main() {
    srand(time(NULL));

    pthread_t consumer_threads[NUM_CONSUMER_THREADS];
    int consumer_thread_ids[NUM_CONSUMER_THREADS];

    pthread_t producer_threads[NUM_PRODUCER_THREADS];
    int producer_thread_ids[NUM_PRODUCER_THREADS];

    sem_init(&mutex, 0, 1);
    sem_init(&consumer_semaphore, 0, CONSUMER_SEM_INIT);
    sem_init(&producer_semaphore, 0, PRODUCER_SEM_INIT);

    for (int i = 0; i < NUM_CONSUMER_THREADS; ++i) {
        consumer_thread_ids[i] = i;
        pthread_create(&consumer_threads[i], NULL, consumer, &consumer_thread_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCER_THREADS; ++i) {
        producer_thread_ids[i] = i;
        pthread_create(&producer_threads[i], NULL, producer, &producer_thread_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCER_THREADS; ++i) {
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMER_THREADS; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }

    sem_destroy(&consumer_semaphore);
    sem_destroy(&producer_semaphore);
    sem_destroy(&mutex);

    return 0;
}

static void* consumer(void* arg) {
    while (1) {
        sem_wait(&consumer_semaphore);
        sem_wait(&mutex);
        int consumed = buffer[current_index--];
        printf("consumed %d\n", consumed);
        sem_post(&mutex);
        sem_post(&producer_semaphore);
    }
    pthread_exit(NULL);
}

static void* producer(void* arg) {
    while (1) {
        sem_wait(&producer_semaphore);
        sem_wait(&mutex);
        buffer[++current_index] = rand();
        sem_post(&mutex);
        sem_post(&consumer_semaphore);
    }
    pthread_exit(NULL);
}
