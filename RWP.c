#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t rw_mutex; // Semaphore to control access to the shared resource
sem_t mutex;    // Semaphore to protect the read_count variable
int read_count = 0; // Number of readers currently accessing the resource
int shared_data = 0; // Shared resource

void *reader(void *arg) {
    int reader_id = *(int *)arg;

    sem_wait(&mutex); // Protect read_count
    read_count++;
    if (read_count == 1) {
        sem_wait(&rw_mutex); // First reader locks the resource
    }
    sem_post(&mutex); // Release protection of read_count

    // Reading the shared resource
    printf("Reader %d: read shared data = %d\n", reader_id, shared_data);
    sleep(1); // Simulate reading time

    sem_wait(&mutex); // Protect read_count
    read_count--;
    if (read_count == 0) {
        sem_post(&rw_mutex); // Last reader unlocks the resource
    }
    sem_post(&mutex); // Release protection of read_count

    return NULL;
}

void *writer(void *arg) {
    int writer_id = *(int *)arg;

    sem_wait(&rw_mutex); // Lock the resource for writing

    // Writing to the shared resource
    shared_data++;
    printf("Writer %d: wrote shared data = %d\n", writer_id, shared_data);
    sleep(1); // Simulate writing time

    sem_post(&rw_mutex); // Unlock the resource

    return NULL;
}

int main() {
    pthread_t readers[5], writers[2];
    int reader_ids[5] = {1, 2, 3, 4, 5};
    int writer_ids[2] = {1, 2};

    // Initialize semaphores
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Create reader and writer threads
    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    // Destroy semaphores
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}