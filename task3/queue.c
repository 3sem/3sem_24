#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2*1024

typedef struct {
    long mtype;
    char mtext[BUFFER_SIZE];
} msgbuf;

int main (int argc, char const *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Use: %s <input> <output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct timespec start, stop;

    const key_t key = 228228;

    pid_t pid = fork();
    int msg_id = msgget(key, IPC_CREAT | 0666);
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pid == 0) {                                                          // recieve
        int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

        msgbuf messa;
        ssize_t bytes_read;

        while (1) {
            bytes_read = msgrcv(msg_id, &messa, BUFFER_SIZE, 0, 0);
            if (bytes_read == -1) {
                perror("Error");
                close(fd);
                return -1;
            }

            if (messa.mtype == 2) {
                if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
                    perror("Removing message queue error");
                }
                break;
            }

            if (write(fd, messa.mtext, bytes_read) == -1) {
                perror("Writing file error(55)");
                close(fd);
                return -1;
            }
        }

        close(fd);
        return EXIT_SUCCESS;
    }

    else {                                                                     // send
        int fd = open(argv[1], O_RDONLY);
        msgbuf messa;
        messa.mtype = 1;

        ssize_t bytes_read;

        while (1) {
            bytes_read = read(fd, messa.mtext, BUFFER_SIZE);

            if (bytes_read != BUFFER_SIZE) {
                if (bytes_read > 0) {
                    if (msgsnd(msg_id, &messa, bytes_read, 0) == -1) {
                        perror("Send message error");
                        close(fd);
                        return -1;
                    }
                }

                messa.mtype = 2;
                if (msgsnd(msg_id, &messa, 0, 0) == -1) {
                    perror("Final message error");
                    close(fd);
                    return -1;
                }
                break;
            }

            if (msgsnd(msg_id, &messa, BUFFER_SIZE, 0) == -1) {
                perror("Sending error");
                close(fd);
                return -1;
            }
        }

        close(fd);
        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &stop);
        double exec_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1e9;
        printf("Время работы: %.4f с\n", exec_time);

        return EXIT_SUCCESS;
    }
    
    return 0;
}