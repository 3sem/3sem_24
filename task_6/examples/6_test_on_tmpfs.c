#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_FILES 10000
#define FILE_SIZE 256

void create_and_delete_files(const char *dir) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_FILES; i++) {
        char filename[256];
        sprintf(filename, "%s/%d", dir, i);

        int fd = open(filename, O_WRONLY | O_CREAT, 0644);
        if (fd < 0) {
            perror("open");
            exit(1);
        }

        char buffer[FILE_SIZE];
        for (int j = 0; j < FILE_SIZE; j++) {
            buffer[j] = 'a';
        }

        write(fd, buffer, FILE_SIZE);
        close(fd);
    }

    for (int i = 0; i < NUM_FILES; i++) {
        char filename[256];
        sprintf(filename, "%s/%d", dir, i);
        unlink(filename);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken in %s: %.6f seconds\n", dir, elapsed);
}

int main() {
    create_and_delete_files("/tmp"); // tmpfs
    create_and_delete_files("/home/user/test"); // traditional file system

    return 0;
}
