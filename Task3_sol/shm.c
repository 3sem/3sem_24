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

#define BUFFER_SIZE 32*1024  
#define SHM_KEY 12345      // Ключ для разделяемой памяти
#define SEM_KEY 54321      // Ключ для семафоров

void handle_error(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

double calculate_time(struct timespec start, struct timespec end){
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void init_semaphore(int sem_id, int sem_num, int init_value){
    if (semctl(sem_id, sem_num, SETVAL, init_value) == -1) {
        handle_error("semctl (SETVAL)");
    }
}

void semaphore_wait(int sem_id, int sem_num){
    struct sembuf operation = {sem_num, -1, 0};
    if (semop(sem_id, &operation, 1) == -1) {
        handle_error("semop (wait)");
    }
}

void semaphore_signal(int sem_id, int sem_num){
    struct sembuf operation = {sem_num, 1, 0};
    if (semop(sem_id, &operation, 1) == -1) {
        handle_error("semop (signal)");
    }
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    // Создаем сегмент разделяемой памяти
    int shm_id = shmget(SHM_KEY, BUFFER_SIZE + sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        handle_error("shmget");
    }

    // Подключаемся к разделяемой памяти
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        handle_error("shmat");
    }

    char *buffer = (char *)shm_ptr;       
    int *data_size = (int *)(buffer + BUFFER_SIZE); 

    // Создаем семафоры
    int sem_id = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1){
        handle_error("semget");
    }

    init_semaphore(sem_id, 0, 1); // Семафор для записи 
    init_semaphore(sem_id, 1, 0); // Семафор для чтения 

    pid_t pid = fork();
    if (pid == -1){
        handle_error("fork");
    }

    if (pid == 0){
        // Дочерний процесс
        FILE *output = fopen(output_file, "wb");
        if (!output) {
            handle_error("fopen (output file)");
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
        // Родительский процесс
        FILE *input = fopen(input_file, "rb");
        if (!input) {
            handle_error("fopen (input file)");
        }

        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time); // Запуск таймера

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

        clock_gettime(CLOCK_MONOTONIC, &end_time); // Остановка таймера

        double elapsed_time = calculate_time(start_time, end_time);
        printf("File transfer completed in %.3f seconds.\n", elapsed_time);

        if (shmdt(shm_ptr) == -1){
            handle_error("shmdt");
        }
        if (shmctl(shm_id, IPC_RMID, NULL) == -1){
            handle_error("shmctl");
        }

        if (semctl(sem_id, 0, IPC_RMID) == -1){
            handle_error("semctl (IPC_RMID)");
        }
    }

    return EXIT_SUCCESS;
}