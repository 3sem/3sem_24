#include <assert.h>
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "shared_mem.h"

int exchangeSharedMemory(int fd_read, int fd_write) {    
    int shmid = shmget(MY_FTOK_KEY, sizeof(SharedData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Error shmget failed");
        return EXIT_FAILURE;
    }

    SharedData *data = (SharedData*)shmat(shmid, NULL, 0);
    if (data == (void*)-1) {
        perror("Error shmat failed");
        return EXIT_FAILURE;
    }

    if (sem_init(&data->sem_write, 1, 1) == -1) {
        perror("Error sem_init failed");
        return EXIT_FAILURE;
    }

    if (sem_init(&data->sem_read, 1, 0) == -1) {
        perror("Error sem_init failed");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while (1) {
            sem_wait(&data->sem_read);
            write(fd_write, data->message, data->len_message);
            sem_post(&data->sem_write);
            
            if (data->len_message == 0) {
                break;
            }
        }
        shmdt(data);
        exit(EXIT_SUCCESS);
    } else {
        while (1) {
            sem_wait(&data->sem_write);
            data->len_message = read(fd_read, data->message, BUF_SIZE);
            sem_post(&data->sem_read);
            
            if (data->len_message == 0) {
                break;
            }
        }
    }

    int status = EXIT_FAILURE;
    wait(&status);
    if (status != EXIT_SUCCESS) {
        perror("Error wait failed");
        exit(EXIT_FAILURE);
    }

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
