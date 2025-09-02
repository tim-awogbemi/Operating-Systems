/*
 * CP386 Assignment 2 - Question 2
 * File: Q2_file.c
 * 
 * Description: Implementation of Collatz conjecture using POSIX shared memory
 * Child process writes sequence to shared memory, parent reads and outputs it
 * 
 * Author: Afo Awogbemi
 * Date: June 12, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/collatz_shm"
#define MAX_SIZE 4096 // Max bytes for output string

void collatz_string(int n, char *buffer) {
    char temp[50];
    while (n != 1) {
        sprintf(temp, "%d, ", n);
        strcat(buffer, temp);
        if (n % 2 == 0)
            n /= 2;
        else
            n = 3 * n + 1;
    }
    sprintf(temp, "1");
    strcat(buffer, temp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <positive_integer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: Please enter a positive integer.\n");
        exit(EXIT_FAILURE);
    }

    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Configure the size
    ftruncate(shm_fd, MAX_SIZE);

    // Map the shared memory
    char *shm_ptr = mmap(0, MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process writes to shared memory
        shm_ptr[0] = '\0'; // Initialize empty string
        collatz_string(n, shm_ptr);
        exit(EXIT_SUCCESS);
    } else {
        // Parent waits for child and reads from shared memory
        wait(NULL);
        printf("Collatz sequence from shared memory: %s\n", shm_ptr);
        // Cleanup
        shm_unlink(SHM_NAME);
    }

    return 0;
}