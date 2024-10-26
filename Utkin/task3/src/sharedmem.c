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

#include "sharedmem.h"
#include "argvs.h"

key_t generate_unique_key(int a) {
    char temp_filename[] = "/tmp/somefileXXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) {
        perror("Error creating temp file");
        return -1;
    }

    key_t key = ftok(temp_filename, a);
    close(fd);
    unlink(temp_filename);
    return key;
}

int DeleteShdata(ShMemData* shdata) {
    if (shdata) {
        int t = shdata->shmid;
        shmdt(shdata);
        if (shmctl(t, IPC_RMID, NULL) == -1) perror("shmctl failed");
    }
    return 0;
}

ShMemData* Init_shdata() {
    key_t key = generate_unique_key(66);
    if (key < 0) {perror("ftok failed"); exit(EXIT_FAILURE);}

    //printf("key: %d\n", key);
    int shmid = shmget(key, sizeof(ShMemData), 0666 | IPC_CREAT);
    if (shmid == -1) perror("shm didnt create");

    ShMemData* shdata = (ShMemData*)shmat(shmid, NULL, 0);
    shdata->shmid = shmid;
    if (shdata == (void*)-1) perror("shmat didnt work");

    shdata->len = 127;
    if (sem_init(&shdata->sem_wr, 1, 1) == -1 || sem_init(&shdata->sem_rd, 1, 0) == -1)
        perror("semaphore failed");
    return shdata;
}

int SharedMemory(int fread, int fwrite, ShMemData* shdata) {
    pid_t pid = fork();

    if (pid < 0) perror("pid didnt create");
    else if (pid > 0) {
        while (shdata->len > 0) {
            sem_wait(&shdata->sem_wr);
            shdata->len = read(fread, shdata->data, BUF_SIZE);
            sem_post(&shdata->sem_rd);
        }
    } else {
        while (1) {
            sem_wait(&shdata->sem_rd);
            if (shdata->len <= 0) break;
            write(fwrite, shdata->data, shdata->len);
            sem_post(&shdata->sem_wr);
        }
        shmdt(shdata);
        exit(0);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WEXITSTATUS(status) != EXIT_SUCCESS) perror("Error waitpid");   
    return EXIT_SUCCESS;
}

int ShMemSendFile(int fread, int fwrite) {
    ShMemData* data = Init_shdata();
    int res = SharedMemory(fread, fwrite, data);
    DeleteShdata(data);
    return res;
}