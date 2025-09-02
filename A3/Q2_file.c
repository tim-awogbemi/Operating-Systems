/*
 * CP386 Assignment 3 - Question 2
 * File: Q2_file.c
 * Author: Afo Awogbemi
 * Date: June 19, 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100

// Structure to store thread/task information
typedef struct {
    char tid[10];         // Thread ID (e.g., T1, T2, etc.)
    int arrival_time;     // Arrival Time (AT)
    int burst_time;       // Burst Time (BT)
    int completion_time;  // Completion Time (CT)
    int turnaround_time;  // Turnaround Time (TAT = CT - AT)
    int waiting_time;     // Waiting Time (WT = TAT - BT)
} ThreadInfo;

// Comparator for sorting threads by arrival time (FCFS rule)
int compareArrival(const void *a, const void *b) {
    ThreadInfo *t1 = (ThreadInfo *)a;
    ThreadInfo *t2 = (ThreadInfo *)b;
    return t1->arrival_time - t2->arrival_time;
}

int main() {
    FILE *file = fopen("sample_in_schedule.txt", "r");
    if (file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    ThreadInfo tasks[MAX_TASKS];
    int count = 0;

    // Read the file line-by-line (format: TID AT BT)
    while (fscanf(file, "%s %d %d", tasks[count].tid,
                  &tasks[count].arrival_time,
                  &tasks[count].burst_time) == 3) {
        count++;
    }
    fclose(file);

    // Sort by arrival time
    qsort(tasks, count, sizeof(ThreadInfo), compareArrival);

    // FCFS Scheduling Simulation
    int current_time = 0;
    float total_tat = 0.0, total_wt = 0.0;

    for (int i = 0; i < count; i++) {
        // If CPU is idle, move current_time to task's arrival
        if (current_time < tasks[i].arrival_time) {
            current_time = tasks[i].arrival_time;
        }

        // Set completion time
        tasks[i].completion_time = current_time + tasks[i].burst_time;

        // Calculate turnaround and waiting time
        tasks[i].turnaround_time = tasks[i].completion_time - tasks[i].arrival_time;
        tasks[i].waiting_time = tasks[i].turnaround_time - tasks[i].burst_time;

        // Advance current time
        current_time = tasks[i].completion_time;

        // Accumulate totals
        total_tat += tasks[i].turnaround_time;
        total_wt += tasks[i].waiting_time;
    }

    // Output Header
    printf("Thread ID\tAT\tBT\tCT\tTAT\tWT\n");

    // Output each task's results
    for (int i = 0; i < count; i++) {
        printf("%s\t\t%d\t%d\t%d\t%d\t%d\n",
               tasks[i].tid,
               tasks[i].arrival_time,
               tasks[i].burst_time,
               tasks[i].completion_time,
               tasks[i].turnaround_time,
               tasks[i].waiting_time);
    }

    // Print averages
    printf("\nAverage Turnaround Time = %.2f\n", total_tat / count);
    printf("Average Waiting Time    = %.2f\n", total_wt / count);

    return 0;
}