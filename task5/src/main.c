#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>

#include "config.h"
#include "child.h"
#include "parent.h"

sem_t* child_sem  = NULL;
sem_t* parent_sem = NULL;

int main(int argc, char* argv[]) {
    #define START_PROCCESS(func) do{                    \
        if (func) {                                     \
            fprintf(stderr, "Error in proccess!\n");    \
            return -1;                                  \
        }                                               \
    } while(0)

    if (argc != 2) {
        fprintf(stderr, "Invalid argument count\n");
        return -1;
    }

    sem_unlink(CHILD_SEMAPHORE_NAME);
    sem_unlink(PARENT_SEMAPHORE_NAME);

    child_sem = sem_open(CHILD_SEMAPHORE_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (child_sem == SEM_FAILED) {
        perror("child_sem sem_open error: ");
        return 1;
    }

    parent_sem = sem_open(PARENT_SEMAPHORE_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (parent_sem == SEM_FAILED) {
        perror("parent_sem sem_open error: ");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork: ");
        return -1;
    }

    if (pid == 0) { // receiver
        START_PROCCESS(child_function());
    }
    else {          // writer
        START_PROCCESS(parent_function(argv[1], pid));
    }

    return 0;

    #undef START_PROCCESS
}