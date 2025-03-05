#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void create_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s", content);
    fclose(file);
}

void read_file(const char *filename) {
    sleep(1); // Wait for 1 second
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s: %s", filename, buffer);
    }
    fclose(file);
}

int main() {
    pid_t child1, child2;

    // Create child1 process
    child1 = fork();
    if (child1 < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child1 == 0) {
        // Child 1 process
        read_file("child1.txt");
        exit(EXIT_SUCCESS);
    }

    // Create child2 process
    child2 = fork();
    if (child2 < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (child2 == 0) {
        // Child 2 process
        read_file("child2.txt");
        exit(EXIT_SUCCESS);
    }

    // Master process
    create_file("child1.txt", "child 1\n");
    create_file("child2.txt", "child 2\n");

    // Wait for both child processes to finish
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    printf("Master process: Both child processes have finished.\n");

    return 0;
}
