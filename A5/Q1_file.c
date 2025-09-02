/*
 * CP386 Assignment 5 - Question 1
 * File: Q1_file.c
 * Author: Afo Awogbemi
 * Date: July Aug 1, 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* Global data structures */
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* Function prototypes */
void initialize_data_structures(int argc, char *argv[]);
void read_maximum_matrix(const char *filename);
bool is_safe_state();
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
void print_state();
void process_command(char *command);

int main(int argc, char *argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Usage: %s <resource 1> <resource 2> <resource 3> <resource 4>\n", argv[0]);
        return 1;
    }

    initialize_data_structures(argc, argv);
    read_maximum_matrix("to_read.txt");

    printf("Banker's Algorithm Simulation\n");
    printf("Available commands:\n");
    printf("  RQ <customer> <r1> <r2> <r3> <r4> - Request resources\n");
    printf("  RL <customer> <r1> <r2> <r3> <r4> - Release resources\n");
    printf("  * - Print current state\n");
    printf("  quit - Exit the program\n\n");

    char command[100];
    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove newline

        if (strcmp(command, "quit") == 0) {
            break;
        }

        process_command(command);
    }

    return 0;
}

void initialize_data_structures(int argc, char *argv[]) {
    // Initialize available resources from command line arguments
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    // Initialize allocation and need matrices to zero
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
            need[i][j] = 0;
        }
    }
}

void read_maximum_matrix(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, "%d,", &maximum[i][j]) != 1) {
                fprintf(stderr, "Error reading maximum matrix from file\n");
                exit(1);
            }
            need[i][j] = maximum[i][j]; // Initially, need = maximum
        }
    }

    fclose(file);
}

bool is_safe_state() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false};
    int safe_sequence[NUMBER_OF_CUSTOMERS];
    int count = 0;

    // Initialize work = available
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    // Find a customer that can finish
    while (count < NUMBER_OF_CUSTOMERS) {
        bool found = false;
        
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }

                if (can_allocate) {
                    // Pretend to allocate resources
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    
                    safe_sequence[count++] = i;
                    finish[i] = true;
                    found = true;
                }
            }
        }

        if (!found) {
            // No safe sequence found
            return false;
        }
    }

    // If we get here, a safe sequence exists
    printf("Safe sequence: ");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d ", safe_sequence[i]);
    }
    printf("\n");
    
    return true;
}

int request_resources(int customer_num, int request[]) {
    // Check if rquest exceeds need
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("Error: Request exceeds maximum need.\n");
            return -1;
        }
    }

    // Check if request exceeds available
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            printf("Request denied: Not enough resources available.\n");
            return -1;
        }
    }

    // Try to allocate resources temporarily
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Check if the new state safe
    if (is_safe_state()) {
        printf("Request granted. System is in safe state.\n");
        return 0;
    } else {
        // Rollback the allocation
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        
        printf("Request denied: Would lead to unsafe state.\n");
        return -1;
    }
}

void release_resources(int customer_num, int release[]) {
    // Check if release exceeds allocation
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            printf("Error: Trying to release more than allocated.\n");
            return;
        }
    }

    // Release the resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }

    printf("Resources released successfully.\n");
}

void print_state() {
    printf("\nCurrent system state:\n");
    
    // Print available resources
    printf("Available resources:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("R%d: %d ", i, available[i]);
    }
    printf("\n\n");
    
    // Print maximum matrix
    printf("Maximum matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("R%d: %d ", j, maximum[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Print allocation matrix
    printf("Allocation matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("R%d: %d ", j, allocation[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Print need matrix
    printf("Need matrix:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("R%d: %d ", j, need[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Check and display if system is in safe state
    printf("Safety check:\n");
    if (is_safe_state()) {
        printf("System is in a safe state.\n");
    } else {
        printf("WARNING: System is in an unsafe state!\n");
    }
    printf("\n");
}

void process_command(char *command) {
    if (strcmp(command, "*") == 0) {
        print_state();
        return;
    }

    char *token = strtok(command, " ");
    if (token == NULL) {
        printf("Invalid command\n");
        return;
    }

    if (strcmp(token, "RQ") == 0 || strcmp(token, "RL") == 0) {
        bool is_request = (strcmp(token, "RQ") == 0);
        int customer_num;
        int request[NUMBER_OF_RESOURCES];

        // Get customer number
        token = strtok(NULL, " ");
        if (token == NULL) {
            printf("Missing customer number\n");
            return;
        }
        customer_num = atoi(token);
        if (customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS) {
            printf("Invalid customer number\n");
            return;
        }

        // Get resource requests
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Missing resource %d\n", i);
                return;
            }
            request[i] = atoi(token);
            if (request[i] < 0) {
                printf("Invalid resource amount\n");
                return;
            }
        }

        if (is_request) {
            request_resources(customer_num, request);
        } else {
            release_resources(customer_num, request);
        }
    } else {
        printf("Invalid command\n");
    }
}