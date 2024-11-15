#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024

void printMemoryInfo(pid_t pid) {
    char filename[256];
    char buffer[BUF_SIZE];
    int fd;

    // Construct the filename for /proc/<pid>/status
    snprintf(filename, sizeof(filename), "/proc/%d/status", pid);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    while (read(fd, buffer, BUF_SIZE) > 0) {
        char *line = buffer;
        while (*line != '\0') {
            if (strncmp(line, "VmPeak:", 7) == 0) {
                printf("VmPeak: %s", line + 8);
            } else if (strncmp(line, "VmSize:", 7) == 0) {
                printf("VmSize: %s", line + 8);
            } else if (strncmp(line, "VmRSS:", 6) == 0) {
                printf("VmRSS: %s", line + 7);
            } else if (strncmp(line, "VmHWM:", 6) == 0) {
                printf("VmHWM: %s", line + 7);
            }
            line += strlen(line) + 1;
        }
    }

    close(fd);
}

void printMemoryMap(pid_t pid) {
    char filename[256];
    char buffer[BUF_SIZE];
    int fd;

    // Construct the filename for /proc/<pid>/maps
    snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    while (read(fd, buffer, BUF_SIZE) > 0) {
        char *line = buffer;
        while (*line != '\0') {
            printf("%s", line);
            line += strlen(line) + 1;
        }
    }

    close(fd);
}

int main() {
    pid_t pid = getpid(); // Get the current process ID

    printf("Process ID: %d\n", pid);

    printMemoryInfo(pid);
    printf("\n");

    printMemoryMap(pid);

    return 0;
}
