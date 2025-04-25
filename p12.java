
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 5 // Size of the buffer
#define NUM_ITEMS 10  // Number of items to produce/consume

int buffer[BUFFER_SIZE]; // Shared buffer
int in = 0, out = 0;     // Indices for producer and consumer

sem_t empty;  // Counts empty buffer slots
sem_t full;   // Counts filled buffer slots
pthread_mutex_t mutex; // Ensures mutual exclusion

void *producer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = i + 1; // Produce an item

        sem_wait(&empty); // Wait if buffer is full
        pthread_mutex_lock(&mutex); // Lock access to buffer

        buffer[in] = item;
        printf("Producer produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); // Unlock buffer access
        sem_post(&full); // Signal that an item is available

        sleep(1); // Simulate time delay
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        sem_wait(&full); // Wait if buffer is empty
        pthread_mutex_lock(&mutex); // Lock access to buffer

        int item = buffer[out];
        printf("\tConsumer consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex); // Unlock buffer access
        sem_post(&empty); // Signal that a buffer slot is free

        sleep(2); // Simulate time delay
    }
    return NULL;
}

int main() {
    pthread_t prodThread, consThread;

    // Initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_create(&prodThread, NULL, producer, NULL);
    pthread_create(&consThread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(prodThread, NULL);
    pthread_join(consThread, NULL);

    // Cleanup
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
