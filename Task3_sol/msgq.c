#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define MSG_KEY 12345      
#define BUFFER_SIZE 1024   
#define MSG_TYPE 1          

struct message{    // Структура сообщения
    long msg_type;               
    char data[BUFFER_SIZE];      
    int data_size;             
};

void handle_error(const char *msg){    // Функция для обработки ошибок
    perror(msg);
    exit(EXIT_FAILURE);
}

double calculate_time(struct timespec start, struct timespec end){ // Функция для расчёта времени в секундах
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    // Создаём очередь сообщений
    int msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msg_id == -1){
        handle_error("msgget");
    }

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

        struct message msg;

        while (1){
            // Получаем сообщение
            if (msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), MSG_TYPE, 0) == -1) {
                handle_error("msgrcv");
            }

            if (msg.data_size == -1) {
                break;
            }

            fwrite(msg.data, 1, msg.data_size, output);
        }

        fclose(output);
        exit(EXIT_SUCCESS);

    } else{
        // Родительский процесс
        FILE *input = fopen(input_file, "rb");
        if (!input) {
            handle_error("fopen (input file)");
        }

        struct message msg;
        msg.msg_type = MSG_TYPE;

        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time); // Запуск таймера

        size_t bytes_read;
        while ((bytes_read = fread(msg.data, 1, BUFFER_SIZE, input)) > 0){
            msg.data_size = bytes_read;

            // Отправляем сообщение
            if (msgsnd(msg_id, &msg, sizeof(msg) - sizeof(long), 0) == -1){
                handle_error("msgsnd");
            }
        }

        fclose(input);

        msg.data_size = -1;
        if (msgsnd(msg_id, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            handle_error("msgsnd");
        }

        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end_time); // Остановка таймера

        double elapsed_time = calculate_time(start_time, end_time);
        printf("File transfer completed in %.3f seconds.\n", elapsed_time);

        if (msgctl(msg_id, IPC_RMID, NULL) == -1){
            handle_error("msgctl");
        }
    }

    return EXIT_SUCCESS;
}