#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ucontext.h>

#include "config.h"
#include "child.h"

static void handler(int signo, siginfo_t *info, void *context) {
    extern sem_t* parent_sem;
    extern sem_t* child_sem;

    sem_wait(child_sem);

    FILE* output_file = fopen(OUTPUT_FILENAME, "ab");
    fwrite(&(info->si_value), sizeof(char), 1, output_file);
    fclose(output_file);

    sem_post(parent_sem);
}

int child_function() {
    extern sem_t* parent_sem;
    extern sem_t* child_sem;

    struct sigaction act = {};
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;

    if (sigaction(1, &act, NULL)) {
        perror("Sigaction: ");
        return -1;
    }

    FILE* output_file = fopen(OUTPUT_FILENAME, "w");
    fclose(output_file);

    sem_post(parent_sem);

    while (1) {}

    return 0;
}