/*
 * CP386 Assignment 2 - Question 1
 * File: Q1_file.c
 * 
 * Description: Implementation of Collatz conjecture using fork() system call
 * The child process generates and outputs the Collatz sequence while the parent waits
 * 
 * Author: Afo Awogbemi
 * Date: June 12, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Function to apply Collatz logic and print the sequence
void collatz(int n) {
    printf("Collatz sequence: ");
    while (n != 1) {
        printf("%d, ", n);
        if (n % 2 == 0)
            n /= 2;
        else
            n = 3 * n + 1;
    }
    printf("1\n"); // Final element
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

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process prints the sequence
        collatz(n);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process waits for child
        wait(NULL);
    }

    return 0;
}