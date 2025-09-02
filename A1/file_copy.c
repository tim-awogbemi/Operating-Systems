/*
* file_copy.c
*
* Copies contents from a source file to a destination file using system calls.
* Handles errors like missing files, permission issues, or disk space errors. 
* 
* Author: Afolabi Awogbemi
* Course: CP-386
* A1 - Q2
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Validate argument count
    if (argc != 3) {
        fprintf(stderr, "Insufficient parameters passed.\n");
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source_file = argv[1];
    const char *dest_file = argv[2];

    // Open the source file for reading
    int src_fd = open(source_file, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }

    // Try to remove the destination file if it already exists
    unlink(dest_file);

    // Create destination file with appropriate permissions
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Error creating destination file");
        close(src_fd);
        return EXIT_FAILURE;
    }

    // Buffer to hold data during copy
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // Read from source and write to destination
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            return EXIT_FAILURE;
        }
    }

    // Check for read error
    if (bytes_read == -1) {
        perror("Error reading from source file");
        close(src_fd);
        close(dest_fd);
        return EXIT_FAILURE;
    }

    // Close file descriptors
    close(src_fd);
    close(dest_fd);

    printf("The contents of file '%s' have been successfully copied into the '%s' file.\n", source_file, dest_file);

    return EXIT_SUCCESS;

}