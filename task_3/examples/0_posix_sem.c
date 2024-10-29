#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main() {
    sem_t *sem = sem_open("/mysem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    pid_t pid;
    for (int i = 0; i < 5; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) { // Child process
            sem_wait(sem);
            printf("Child(%d) is in critical section.\n", i);
            sleep(1);
            printf("Child(%d) exiting critical section.\n", i);
            sem_post(sem);
            exit(0);
        }
    }

    // Parent process
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink("/mysem");
}
