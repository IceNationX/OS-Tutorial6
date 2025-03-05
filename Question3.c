#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define ARRAY_LENGTH 5

// Global variables
int moving_sum[ARRAY_LENGTH] = {0};
sem_t moving_sum_semaphore[ARRAY_LENGTH];

// Struct to pass data to thread
typedef struct {
    int number;
    int index;
} ThreadData;

// Function to calculate factorial
unsigned long long calculate_factorial(int n) {
    if (n < 0) return 0;
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Thread function
void* factorial_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // Calculate factorial
    unsigned long long fact = calculate_factorial(data->number);
    
    // Semaphore logic for moving sum
    if (data->index > 0) {
        // Wait until previous index has a value
        while (moving_sum[data->index - 1] == 0) {
            // Signal and wait to allow other threads access
            sem_wait(&moving_sum_semaphore[data->index - 1]);
            sem_post(&moving_sum_semaphore[data->index - 1]);
        }
        
        // Safely access and update moving_sum
        sem_wait(&moving_sum_semaphore[data->index]);
        moving_sum[data->index] = moving_sum[data->index - 1] + fact;
        sem_post(&moving_sum_semaphore[data->index]);
    } else {
        // First index just stores its own factorial
        sem_wait(&moving_sum_semaphore[data->index]);
        moving_sum[data->index] = fact;
        sem_post(&moving_sum_semaphore[data->index]);
    }
    
    return NULL;
}

int main(void) {
    int numbers[ARRAY_LENGTH];
    pthread_t threads[ARRAY_LENGTH];
    ThreadData thread_data[ARRAY_LENGTH];
    
    // Initialize semaphores
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        sem_init(&moving_sum_semaphore[i], 0, 1);
    }
    
    // Prompt for numbers
    printf("Enter 5 numbers:\n");
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        printf("Number %d: ", i + 1);
        scanf("%d", &numbers[i]);
    }
    
    // Create threads
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        thread_data[i].number = numbers[i];
        thread_data[i].index = i;
        
        pthread_create(&threads[i], NULL, factorial_thread, &thread_data[i]);
    }
    
    // Join threads
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Print moving_sum
    printf("\nMoving Sum:\n");
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        printf("Index %d: %d\n", i, moving_sum[i]);
    }
    
    // Destroy semaphores
    for (int i = 0; i < ARRAY_LENGTH; i++) {
        sem_destroy(&moving_sum_semaphore[i]);
    }
    
    return 0;
}
