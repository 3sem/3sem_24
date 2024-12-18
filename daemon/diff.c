#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

#define DIFF_DIRECTORY "./backup_daemon"  // Directory where your diff files are stored
#define TARGET_FILE "./backup_daemon/test_file.txt"  // The file being "played back"

// Function to apply a single diff to the target file
void apply_diff(const char *file, const char *diff) {
    char command[512];
    snprintf(command, sizeof(command), "patch %s < %s", file, diff);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Error applying diff: %s\n", diff);
    } else {
        printf("Applied diff: %s\n", diff);
    }
}

// Comparison function to sort diff files by their numeric filename in descending order
int compare_numbers(const void *a, const void *b) {
    const char *file_a = *(const char **)a;
    const char *file_b = *(const char **)b;

    // Extract the numeric part from the filenames (e.g., "1234.diff" -> 1234)
    int num_a = atoi(file_a);
    int num_b = atoi(file_b);

    // Compare the numbers (descending order)
    return num_b - num_a;  // Reverse the order for descending sort
}

// Function to playback the changes, applying diffs in reverse order based on numeric filenames
void playback_changes(const char *target_file, int num_diffs) {
    DIR *dir;
    struct dirent *entry;
    char *diff_files[100];  // Array to store diff file names
    int count = 0;

    // Open the directory containing diff files
    dir = opendir(DIFF_DIRECTORY);
    if (dir == NULL) {
        perror("Failed to open diff directory");
        return;
    }

    // Read diff files from the directory and store them in an array
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".diff") != NULL) {
            // Allocate memory for the diff file name and store it
            diff_files[count] = strdup(entry->d_name);
            count++;
        }
    }

    // Close the directory
    closedir(dir);

    // If no diff files found, return
    if (count == 0) {
        printf("No diff files found in the directory.\n");
        return;
    }

    // Sort the files by their numeric filenames in reverse order (most recent first)
    qsort(diff_files, count, sizeof(char *), compare_numbers);

    // Apply the diffs in reverse order, up to the specified number
    for (int i = 0; i < num_diffs && i < count; i++) {
        char diff_path[512];
        snprintf(diff_path, sizeof(diff_path), "%s/%s", DIFF_DIRECTORY, diff_files[i]);

        printf("Applying diff: %s\n", diff_path);
        apply_diff(target_file, diff_path);

        free(diff_files[i]);  // Free the allocated memory for the diff file name
    }
}

int main(int argc, char *argv[]) {
    int num_diffs = 0;

    // Parse the command-line argument for the number of diffs
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_diffs>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_diffs = atoi(argv[1]);
    if (num_diffs <= 0) {
        fprintf(stderr, "Error: Number of diffs must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Example of playback in reverse order with the specified number of diffs
    printf("Starting reverse playback with %d diffs...\n", num_diffs);
    playback_changes(TARGET_FILE, num_diffs);

    return EXIT_SUCCESS;
}

