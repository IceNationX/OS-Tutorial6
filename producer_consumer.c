#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5
#define NUMBERS 10

int buffer[BUFFER_SIZE] = {0}; // Shared buffer
int count = 0; // Number of items in the buffer

sem_t mutex; // Semaphore for mutual exclusion
sem_t empty; // Semaphore to track empty slots in the buffer
sem_t full;  // Semaphore to track filled slots in the buffer

void* producer(void* arg) {
    int* numbers = (int*)arg;
    for (int i = 0; i < NUMBERS; i++) {
        sleep((rand() % 3) + 1); // Random delay before producing
        sem_wait(&empty); // Wait for an empty slot
        sem_wait(&mutex); // Enter critical section

        // Add the number to the buffer
        buffer[count] = numbers[i];
        printf("Produced %d\n", numbers[i]);
        count++;

        sem_post(&mutex); // Exit critical section
        sem_post(&full);  // Signal that a slot is filled
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < NUMBERS; i++) {
        sleep((rand() % 3) + 1); // Random delay before consuming
        sem_wait(&full);  // Wait for a filled slot
        sem_wait(&mutex); // Enter critical section

        // Consume the number from the buffer
        int number = buffer[count - 1];
        buffer[count - 1] = 0; // Mark the slot as empty
        printf("Consumed %d\n", number);
        count--;

        sem_post(&mutex); // Exit critical section
        sem_post(&empty); // Signal that a slot is empty
    }
    return NULL;
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    int numbers[NUMBERS];
    printf("Enter %d numbers:\n", NUMBERS);
    for (int i = 0; i < NUMBERS; i++) {
        scanf("%d", &numbers[i]);
    }

    // Initialize semaphores
    sem_init(&mutex, 0, 1); // Mutex for critical section
    sem_init(&empty, 0, BUFFER_SIZE); // Initially, all slots are empty
    sem_init(&full, 0, 0); // Initially, no slots are filled

    // Create producer and consumer threads
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, (void*)numbers);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for both threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Print the final state of the buffer
    printf("Final buffer contents:\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
