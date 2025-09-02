/*
* file_directory.c
*
* Implements various file and directory operations using Linux system calls.
*
* Author: Afolabi Awogbemi
* Course: CP-386 - Spring 2025
* Assignment 1 - Question 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

// Function to create a directory with given name and permissions
void create_directory(const char *dir_name, mode_t mode) {
    if (mkdir(dir_name, mode) == -1) {
        perror("Error creating directory");
    } else {
        printf("Directory '%s' created successfully.\n", dir_name);
    }
}

// Function to create a file and write specific content to it
void create_write_file(const char *file_name, const char *content) {
    int fd = open (file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644 );
    if (fd == -1) {
        perror("Error opening file");
        return;
    }
    if (write(fd, content, strlen(content)) == -1) {
        perror("Error writing to file");
        close(fd);
        return;
    }
    close(fd);
    printf("File '%s' created and content written successfully. \n", file_name);
}

// Function to read memory info from /proc/meminfo and write it to a file
void read_proc_mem_info_to_file(const char *file_name) {
    FILE *src = fopen("/proc/meminfo", "r");
    if (!src) {
        perror("Error opening /proc/meminfo");
        return;
    }

    FILE *dest = fopen(file_name, "w");
    if (!dest) {
        perror("Error creating output file");
        fclose(src);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), src)) {
        fputs(buffer, dest);
    }

    fclose(src);
    fclose(dest);
    printf("Memory info copied to '%s' successfully. \n", file_name);
}

// Recursive function to list directory contents
void directory_listing(const char *start_dir) {
    DIR *dir = opendir(start_dir);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. 
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", start_dir, entry->d_name);
        printf("%s\n", path);

        // Recurse if it's a directory
        struct stat statbuf;
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            directory_listing(path);
        }
    }

    closedir(dir);

}

// Recursive helper to remove directory and contents
void remove_directory_recursive(const char *dir_name) {
    DIR *dir = opendir(dir_name);
    if (!dir) {
        perror("Error opening directory");
        return;
    }
    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
        struct stat statbuf;
        if (stat(path, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                remove_directory_recursive(path);
            } else {
                if (remove(path) != 0) perror("Error removing file");
            }
        }
    }

    closedir(dir);
    if (rmdir(dir_name) != 0) {
        perror("Error removing directory");
    } else {
        printf("Directory '%s' deleted successfully. \n", dir_name);
    }
}

// Function to remove directory after confirmation
void remove_directory(const char *dir_name) {
    char confirm;
    printf("Warning: you are about to delete the directory '%s' and all its recursively. Proceed? (y/n): ", dir_name);
    scanf("%c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        remove_directory_recursive(dir_name);
    } else {
        printf("Deletion cancelled.\n");
    }
}

// Main function to provide menu and user interface
int main() {
    int choice;
    char dir_name[256], file_name[256];
    while (1) {
        printf("\nMenu:\n");
        printf("1. Create Directory\n");
        printf("2. Create and Write File\n");
        printf("3. Read /proc/meminfo to File\n");
        printf("4. List Directory Contents\n");
        printf("5. Remove Directory\n");
        printf("99, Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter directory name: ");
                scanf("%s", dir_name);
                create_directory(dir_name, 0755);
                break;
            case 2:
                printf("Enter file name: ");
                scanf("%s", file_name);
                create_write_file(file_name, "Operating systems iS Fun !!!\n");
                break;
            case 3:
                printf("Enter file name to save memory info: ");
                scanf("%s", file_name);
                read_proc_mem_info_to_file(file_name);
                break;
            case 4:
                printf("Enter starting directory: ");
                scanf("%s", dir_name);
                directory_listing(dir_name);
                break;
            case 5:
                printf("Enter directory name to delete: ");
                scanf("%s", dir_name);
                remove_directory(dir_name);
                break;
            case 99:
                printf("Exiting program.\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}   
