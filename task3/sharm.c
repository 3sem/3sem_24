#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 64*1024  
#define SHM_KEY 1337228      
#define SEM_KEY 228    

void handle_error(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void init_semaphore(int sem_id, int sem_num, int init_value){
    if (semctl(sem_id, sem_num, SETVAL, init_value) == -1) {
        handle_error("semctl (SETVAL)");
    }
}

void semaphore_wait(int sem_id, int sem_num){
    struct sembuf operation = {sem_num, -1, 0};
    if (semop(sem_id, &operation, 1) == -1) {
        handle_error("semafor (wait) error");
    }
}

void semaphore_signal(int sem_id, int sem_num){
    struct sembuf operation = {sem_num, 1, 0};
    if (semop(sem_id, &operation, 1) == -1) {
        handle_error("semafor (signal) error");
    }
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Use: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    int shm_id = shmget(SHM_KEY, BUFFER_SIZE + sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        handle_error("shmget error");
    }

    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        handle_error("shmat error");
    }

    char *buffer = (char *)shm_ptr;       
    int *data_size = (int *)(buffer + BUFFER_SIZE); 

    int sem_id = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1){
        handle_error("semget error");
    }

    init_semaphore(sem_id, 0, 1);  
    init_semaphore(sem_id, 1, 0); 

    pid_t pid = fork();
    if (pid == -1){
        handle_error("fork error");
    }

    if (pid == 0){
        FILE *output = fopen(output_file, "wb");
        if (!output) {
            handle_error("fopen (output file) error");
        }

        while (1){
            semaphore_wait(sem_id, 1);

            if (*data_size == -1){
                break;
            }

            fwrite(buffer, 1, *data_size, output);

            semaphore_signal(sem_id, 0);
        }

        fclose(output);
        exit(EXIT_SUCCESS);

    } else{
        FILE *input = fopen(input_file, "rb");
        if (!input) {
            handle_error("fopen (input file) error");
        }

        struct timespec start, stop;
        clock_gettime(CLOCK_MONOTONIC, &start); 

        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0){
            semaphore_wait(sem_id, 0);

            *data_size = bytes_read;

            semaphore_signal(sem_id, 1);
        }

        fclose(input);

        semaphore_wait(sem_id, 0);
        *data_size = -1;
        semaphore_signal(sem_id, 1);

        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &stop);
        double exec_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1e9;
        printf("Время работы %.4f с\n", exec_time);

        if (shmdt(shm_ptr) == -1){
            handle_error("shmdt error");
        }
        if (shmctl(shm_id, IPC_RMID, NULL) == -1){
            handle_error("shmctl error");
        }

        if (semctl(sem_id, 0, IPC_RMID) == -1){
            handle_error("semctl (IPC_RMID) error");
        }
    }

    return EXIT_SUCCESS;
}