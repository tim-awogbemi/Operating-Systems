/*
 * CP386 Assignment 3 - Question 1
 * File: Q1_file.c
 * Author: Afo Awogbemi
 * Date: June 19, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define SIZE 9
#define NUM_THREADS 11

// Structure to hold starting row and column for each thread
typedef struct {
    int row;
    int column;
} parameters;

// Shared Sudoku grid
int sudoku[SIZE][SIZE];

// Array to hold validation results from threads (1 = valid, 0 = invalid)
int results[NUM_THREADS] = {0};

// Function to read Sudoku grid from file
void read_sudoku_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            fscanf(file, "%d", &sudoku[i][j]);

    fclose(file);
}

// Function to check if digits 1–9 appear exactly once
int is_valid_set(int *arr) {
    int seen[SIZE + 1] = {0}; // Index 0 unused

    for (int i = 0; i < SIZE; ++i) {
        int val = arr[i];
        if (val < 1 || val > 9 || seen[val])
            return 0; // Duplicate or out-of-range number
        seen[val] = 1;
    }
    return 1; // All digits are unique and between 1–9
}

// Thread fr validating row
void *validate_row(void *param) {
    int valid = 1;
    for (int i = 0; i < SIZE && valid; ++i) {
        int row[SIZE];
        for (int j = 0; j < SIZE; ++j)
            row[j] = sudoku[i][j];
        if (!is_valid_set(row)) {
            valid = 0;
        }
    }
    results[0] = valid;
    pthread_exit(NULL);
}

// Thread function for validating a column
void *validate_column(void *param) {
    for (int i = 0; i < SIZE; ++i) {
        int col[SIZE];
        for (int j = 0; j < SIZE; ++j)
            col[j] = sudoku[j][i];

        if (!is_valid_set(col)) {
            pthread_exit(NULL);
        }
    }
    results[1] = 1;
    pthread_exit(NULL);
}

// Thread function for validating a 3x3 subgrid
void *validate_subgrid(void *param) {
    parameters *p = (parameters *)param;
    int startRow = p->row;
    int startCol = p->column;

    int values[SIZE];
    int idx = 0;

    for (int i = startRow; i < startRow + 3; ++i) {
        for (int j = startCol; j < startCol + 3; ++j) {
            values[idx++] = sudoku[i][j];
        }
    }

    int threadIndex = 2 + (startRow / 3) * 3 + (startCol / 3); // unique index for each subgrid
    if (is_valid_set(values)) {
        results[threadIndex] = 1;
    }

    free(param); // Free allocated memory
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Load Sudoku puzzle from file
    read_sudoku_from_file("sample_in_Sudoku.txt");

    // Create thread for row validation
    pthread_create(&threads[0], NULL, validate_row, NULL);

    // Create thread for column validation
    pthread_create(&threads[1], NULL, validate_column, NULL);

    // Create 9 threads for each 3x3 subgrid
    int threadIndex = 2;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            parameters *data = (parameters *)malloc(sizeof(parameters));
            data->row = i;
            data->column = j;
            pthread_create(&threads[threadIndex++], NULL, validate_subgrid, data);
        }
    }

    // Join all threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Check results
    int isValid = 1;
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (results[i] != 1) {
            isValid = 0;
            break;
        }
    }

    // Final result
    if (isValid)
        printf("Valid Sudoku Puzzle\n");
    else
        printf("Not a Valid Sudoku Puzzle\n");

    return 0;
}
