#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

#include "config.h"
#include "parent.h"

int parent_function(const char* filename, pid_t pid) {
    assert(filename);

    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Fopen: ");
        return -1;
    }

    struct stat file_stat = {};
    if (stat(filename, &file_stat)) {
        perror("Stat: ");
        return -1;
    }

    char* buffer = (char*) calloc(file_stat.st_size + 1, sizeof(char));
    if (!buffer) {
        perror("Calloc: ");
        return -1;
    }

    if (file_stat.st_size != fread(buffer, sizeof(char), file_stat.st_size, file)) {
        perror("Fread: ");
        return -1;
    }

    extern sem_t* parent_sem;
    extern sem_t* child_sem;

    union sigval info = {}; 

    for (size_t i = 0; i < file_stat.st_size; i++) {
        info.sival_int = buffer[i];
        sem_wait(parent_sem);
        if(sigqueue(pid, 1, info)) {
            perror("Sigqueue: ");
            return -1;
        }
        sem_post(child_sem);
    }

    sem_wait(parent_sem);
    kill(pid, SIGKILL);

    sem_close(parent_sem);
    sem_close(child_sem);

    return 0;
}