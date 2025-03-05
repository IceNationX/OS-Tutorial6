#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define NUMBERS_COUNT 5

// Global variables
unsigned long long total_sum = 0;
sem_t total_sum_semaphore;

// Function to calculate factorial
unsigned long long calculate_factorial(int n) {
    if (n < 0) return 0; // Factorial is not defined for negative numbers
    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Thread function
void* factorial_thread(void* arg) {
    int number = *(int*)arg;
    
    // Calculate factorial
    unsigned long long fact = calculate_factorial(number);
    
    // Add to total_sum in a thread-safe manner
    sem_wait(&total_sum_semaphore); // Enter critical section
    total_sum += fact;
    sem_post(&total_sum_semaphore); // Exit critical section
    
    // Print the current factorial value
    printf("Factorial of %d: %llu\n", number, fact);
    
    return NULL;
}

int main(void) {
    int numbers[NUMBERS_COUNT];
    pthread_t threads[NUMBERS_COUNT];
    
    // Prompt user for five numbers
    printf("Enter 5 numbers:\n");
    for (int i = 0; i < NUMBERS_COUNT; i++) {
        printf("Number %d: ", i + 1);
        scanf("%d", &numbers[i]);
    }
    
    // Write numbers to numbers.txt
    FILE* numbers_file = fopen("numbers.txt", "w");
    if (numbers_file == NULL) {
        perror("Failed to open numbers.txt");
        return 1;
    }
    for (int i = 0; i < NUMBERS_COUNT; i++) {
        fprintf(numbers_file, "%d\n", numbers[i]);
    }
    fclose(numbers_file);
    
    // Fork a child process
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (child_pid == 0) {
        // Child process
        
        // Initialize semaphore
        sem_init(&total_sum_semaphore, 0, 1);
        
        // Read numbers from numbers.txt
        FILE* numbers_file = fopen("numbers.txt", "r");
        if (numbers_file == NULL) {
            perror("Failed to open numbers.txt");
            return 1;
        }
        for (int i = 0; i < NUMBERS_COUNT; i++) {
            fscanf(numbers_file, "%d", &numbers[i]);
        }
        fclose(numbers_file);
        
        // Create threads to calculate factorials
        for (int i = 0; i < NUMBERS_COUNT; i++) {
            if (pthread_create(&threads[i], NULL, factorial_thread, &numbers[i]) != 0) {
                perror("Failed to create thread");
                return 1;
            }
        }
        
        // Join threads
        for (int i = 0; i < NUMBERS_COUNT; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }
        
        // Write total_sum to sum.txt
        FILE* sum_file = fopen("sum.txt", "w");
        if (sum_file == NULL) {
            perror("Failed to open sum.txt");
            return 1;
        }
        fprintf(sum_file, "%llu\n", total_sum);
        fclose(sum_file);
        
        // Destroy semaphore
        sem_destroy(&total_sum_semaphore);
        
        return 0; // Child process terminates
    } else {
        // Parent process
        
        // Wait for child process to terminate
        wait(NULL);
        
        // Read total_sum from sum.txt
        FILE* sum_file = fopen("sum.txt", "r");
        if (sum_file == NULL) {
            perror("Failed to open sum.txt");
            return 1;
        }
        unsigned long long total_sum;
        fscanf(sum_file, "%llu", &total_sum);
        fclose(sum_file);
        
        // Print the total sum
        printf("Total sum of factorials: %llu\n", total_sum);
        
        return 0;
    }
}
