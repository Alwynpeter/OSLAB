#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 1

// Shared buffer
int buffer[BUFFER_SIZE];
int in = 0, out = 0;  // Indices for producer and consumer

// Semaphores
sem_t mutex;  // Protects access to the shared buffer
sem_t empty;   // Tracks empty slots in the buffer
sem_t full;    // Tracks filled slots in the buffer

void* producer(void* arg) {
    int item;
    
    while (1) {
        item = rand() % 100;  // Produce an item (random number)
        
        sem_wait(&empty);  // Wait for an empty slot
        sem_wait(&mutex);  // Acquire the mutex to access the buffer
        
        // Produce item and put it in the buffer
        buffer[in] = item;
        printf("Producer produced item %d at index %d\n", item, in);
        
        in = (in + 1) % BUFFER_SIZE;  // Circular buffer index
        
        sem_post(&mutex);  // Release the mutex
        sem_post(&full);   // Signal that a new item is available
        
        sleep(1);  // Simulate time taken to produce
    }

    return NULL;
}

void* consumer(void* arg) {
    int item;
    
    while (1) {
        sem_wait(&full);   // Wait for a filled slot
        sem_wait(&mutex);  // Acquire the mutex to access the buffer
        
        // Consume the item from the buffer
        item = buffer[out];
        printf("Consumer consumed item %d from index %d\n", item, out);
        
        out = (out + 1) % BUFFER_SIZE;  // Circular buffer index
        
        sem_post(&mutex);  // Release the mutex
        sem_post(&empty);  // Signal that an empty slot is available
        
        sleep(2);  // Simulate time taken to consume
    }

    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    
    // Initialize semaphores
    sem_init(&mutex, 0, 1);  // Binary semaphore for mutual exclusion
    sem_init(&empty, 0, BUFFER_SIZE);  // Initially, all slots are empty
    sem_init(&full, 0, 0);   // Initially, no slots are filled

    // Create producer and consumer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Clean up semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
