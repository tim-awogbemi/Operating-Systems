/*
 * CP386 Assignment 2 - Question 3
 * File: Q3_file.c
 * 
 * Description: Multithreaded program that calculates statistical values (average, min, max)
 * Uses three worker threads to calculate statistics concurrently
 * 
 * Author: Afo Awogbemi
 * Date: June 12, 2025
 */

#include <stdio.h>      // For printf, fprintf functions
#include <stdlib.h>     // For atoi, malloc, free functions
#include <pthread.h>    // For pthread functions
#include <string.h>     // For string operations
#include <limits.h>     // For INT_MAX, INT_MIN constants

// Structure to hold the array of numbers and its size
typedef struct {
    int* numbers;       // Array of numbers
    int count;         // Number of elements in the array
} NumberData;

// Global variables to store statistical results
double global_average = 0.0;    // Average of the numbers
int global_minimum = INT_MAX;   // Minimum value in the array
int global_maximum = INT_MIN;   // Maximum value in the array

// Pointer to shared data structure
NumberData* shared_data = NULL;

/*
 * Function: is_valid_integer
 * Purpose: Validates if the input string represents a valid integer (positive or negative)
 * Parameters: str - input string to validate
 * Returns: 1 if valid integer, 0 otherwise
 */
int is_valid_integer(const char* str) {
    // Check if string is empty or NULL
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    int start = 0;
    // Allow negative sign at the beginning
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
        // Check if there's at least one digit after the sign
        if (str[1] == '\0') {
            return 0;
        }
    }
    
    // Check each character to ensure it's a digit
    for (int i = start; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    
    return 1;
}

/*
 * Function: calculate_average
 * Purpose: Worker thread function to calculate the average of numbers
 * Parameters: arg - thread argument (not used)
 * Returns: NULL
 */
void* calculate_average(void* arg) {
    printf("Average calculation thread started...\n");
    
    // Calculate sum of all numbers
    long long sum = 0;  // Use long long to prevent overflow
    for (int i = 0; i < shared_data->count; i++) {
        sum += shared_data->numbers[i];
    }
    
    // Calculate average and store in global variable
    global_average = (double)(sum / shared_data->count);
    
    printf("Average calculation thread completed.\n");
    pthread_exit(NULL);
}

/*
 * Function: find_minimum
 * Purpose: Worker thread function to find the minimum value
 * Parameters: arg - thread argument (not used)
 * Returns: NULL
 */
void* find_minimum(void* arg) {
    printf("Minimum calculation thread started...\n");
    
    // Initialize minimum with first element
    int min = shared_data->numbers[0];
    
    // Find minimum value in the array
    for (int i = 1; i < shared_data->count; i++) {
        if (shared_data->numbers[i] < min) {
            min = shared_data->numbers[i];
        }
    }
    
    // Store result in global variable
    global_minimum = min;
    
    printf("Minimum calculation thread completed.\n");
    pthread_exit(NULL);
}

/*
 * Function: find_maximum
 * Purpose: Worker thread function to find the maximum value
 * Parameters: arg - thread argument (not used)
 * Returns: NULL
 */
void* find_maximum(void* arg) {
    printf("Maximum calculation thread started...\n");
    
    // Initialize maximum with first element
    int max = shared_data->numbers[0];
    
    // Find maximum value in the array
    for (int i = 1; i < shared_data->count; i++) {
        if (shared_data->numbers[i] > max) {
            max = shared_data->numbers[i];
        }
    }
    
    // Store result in global variable
    global_maximum = max;
    
    printf("Maximum calculation thread completed.\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Error checking: Ensure at least one number is provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number1> <number2> ... <numberN>\n", argv[0]);
        fprintf(stderr, "Example: %s 90 81 78 95 79 72 85\n", argv[0]);
        exit(1);
    }
    
    // Calculate number of integers provided
    int num_count = argc - 1;
    
    // Allocate memory for the numbers array
    int* numbers = (int*)malloc(num_count * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    // Parse and validate command line arguments
    printf("Parsing %d numbers from command line...\n", num_count);
    for (int i = 0; i < num_count; i++) {
        // Validate each argument is a valid integer
        if (!is_valid_integer(argv[i + 1])) {
            fprintf(stderr, "Error: '%s' is not a valid integer\n", argv[i + 1]);
            free(numbers);
            exit(1);
        }
        
        // Convert string to integer and store
        numbers[i] = atoi(argv[i + 1]);
        printf("Number %d: %d\n", i + 1, numbers[i]);
    }
    
    // Initialize shared data structure
    shared_data = (NumberData*)malloc(sizeof(NumberData));
    if (shared_data == NULL) {
        fprintf(stderr, "Error: Memory allocation for shared data failed\n");
        free(numbers);
        exit(1);
    }
    shared_data->numbers = numbers;
    shared_data->count = num_count;
    
    // Display the input numbers
    printf("\nInput numbers: ");
    for (int i = 0; i < num_count; i++) {
        printf("%d", numbers[i]);
        if (i < num_count - 1) printf(", ");
    }
    printf("\n\n");
    
    // Create three worker threads
    pthread_t avg_thread, min_thread, max_thread;
    
    printf("Creating worker threads...\n");
    
    // Create thread for average calculation
    if (pthread_create(&avg_thread, NULL, calculate_average, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create average calculation thread\n");
        free(numbers);
        free(shared_data);
        exit(1);
    }
    
    // Create thread for minimum calculation
    if (pthread_create(&min_thread, NULL, find_minimum, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create minimum calculation thread\n");
        free(numbers);
        free(shared_data);
        exit(1);
    }
    
    // Create thread for maximum calculation
    if (pthread_create(&max_thread, NULL, find_maximum, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create maximum calculation thread\n");
        free(numbers);
        free(shared_data);
        exit(1);
    }
    
    printf("All worker threads created successfully.\n");
    printf("Main thread waiting for workers to complete...\n\n");
    
    // Wait for all worker threads to complete
    if (pthread_join(avg_thread, NULL) != 0) {
        fprintf(stderr, "Error: Failed to join average calculation thread\n");
    }
    
    if (pthread_join(min_thread, NULL) != 0) {
        fprintf(stderr, "Error: Failed to join minimum calculation thread\n");
    }
    
    if (pthread_join(max_thread, NULL) != 0) {
        fprintf(stderr, "Error: Failed to join maximum calculation thread\n");
    }
    
    printf("All worker threads have completed.\n\n");
    
    // Output the statistical results
    printf("=== STATISTICAL RESULTS ===\n");
    printf("The average value is %.0f\n", global_average);
    printf("The minimum value is %d\n", global_minimum);
    printf("The maximum value is %d\n", global_maximum);
    printf("===========================\n");
    
    // Clean up allocated memory
    free(numbers);
    free(shared_data);
    
    printf("\nProgram completed successfully.\n");
    return 0;
}