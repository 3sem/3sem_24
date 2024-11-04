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
#include <sys/time.h>

#define BUFFER_SIZE     (64 * 1024)
#define MSG_TYPE_DATA   1
#define MSG_TYPE_END    2
#define FILE_PERMS      0666
typedef struct {
    long mtype;
    char mtext[BUFFER_SIZE];
} MessageBuffer;

static int send_file(const char *input_file, int msqid);
static int receive_file(const char *output_file, int msqid);
static void print_usage(const char *program_name);
static double calculate_elapsed_time(struct timeval *start, struct timeval *end);

static int send_file(const char *input_file, int msqid) {
    int fp = open(input_file, O_RDONLY);
    if (fp == -1) {
        perror("Error opening input file");
        return -1;
    }

    MessageBuffer msg = {
        .mtype = MSG_TYPE_DATA
    };
    
    ssize_t bytes_read;
    while (1) {
        bytes_read = read(fp, msg.mtext, BUFFER_SIZE);
        if (bytes_read == -1) {
            perror("Error reading from file");
            close(fp);
            return -1;
        }

        if (bytes_read != BUFFER_SIZE) {
            if (bytes_read > 0) {
                if (msgsnd(msqid, &msg, bytes_read, 0) == -1) {
                    perror("Error sending message");
                    close(fp);
                    return -1;
                }
            }
            
            msg.mtype = MSG_TYPE_END;
            if (msgsnd(msqid, &msg, 0, 0) == -1) {
                perror("Error sending final message");
                close(fp);
                return -1;
            }
            break;
        } 

        if (msgsnd(msqid, &msg, BUFFER_SIZE, 0) == -1) {
            perror("Error sending message");
            close(fp);
            return -1;
        }
    }

    if (close(fp) == -1) {
        perror("Error closing input file");
        return -1;
    }
    
    return 0;
}

static int receive_file(const char *output_file, int msqid) {
    int fp = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERMS);
    if (fp == -1) {
        perror("Error opening output file");
        return -1;
    }

    MessageBuffer msg;
    ssize_t bytes_read;
    
    while (1) {
        bytes_read = msgrcv(msqid, &msg, BUFFER_SIZE, 0, 0);
        if (bytes_read == -1) {
            perror("Error receiving message");
            close(fp);
            return -1;
        }

        if (msg.mtype == MSG_TYPE_END) {
            if (msgctl(msqid, IPC_RMID, NULL) == -1) {
                perror("Error removing message queue");
            }
            break;
        }

        if (write(fp, msg.mtext, bytes_read) == -1) {
            perror("Error writing to file");
            close(fp);
            return -1;
        }
    }

    if (close(fp) == -1) {
        perror("Error closing output file");
        return -1;
    }
    
    return 0;
}

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", program_name);
}

static double calculate_elapsed_time(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) + 
           (end->tv_usec - start->tv_usec) / 1000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    key_t queue_key = ftok(argv[1], 1);
    int msqid = msgget(queue_key, FILE_PERMS | IPC_CREAT);
    if (msqid == -1) {
        perror("Error creating message queue");
        return EXIT_FAILURE;
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("Error creating child process");
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        if (receive_file(argv[2], msqid) == -1) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } 
    if (send_file(argv[1], msqid) == -1) {
        return EXIT_FAILURE;
    }
    
    wait(NULL);
    gettimeofday(&end, NULL);
    
    printf("Time spent: %.6f seconds\n", 
           calculate_elapsed_time(&start, &end));
    
    return EXIT_SUCCESS;
}
