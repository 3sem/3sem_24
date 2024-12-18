#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#define WORKING_DIR "./test_working_dir"
#define FILE_NAME "test_file.txt"

void create_directory(const char *dir) {
    if (mkdir(dir, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}

void modify_file(const char *file_name) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    printf("Current working directory: %s\n", cwd);

    while (1) {
        FILE *file = fopen(file_name, "a");
        if (!file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        time_t now = time(NULL);
        fprintf(file, "Modified at: %s", ctime(&now));
        fclose(file);

        printf("Modified file: %s/%s\n", cwd, file_name);
        sleep(5); // Modify the file every 5 seconds
    }
}

int main() {
    printf("Test process started. PID: %d\n", getpid());

    // Create and change to the working directory
    create_directory(WORKING_DIR);

    if (chdir(WORKING_DIR) == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    printf("Working directory: %s\n", WORKING_DIR);

    // Modify the single file in the working directory
    modify_file(FILE_NAME);

    return 0;
}


