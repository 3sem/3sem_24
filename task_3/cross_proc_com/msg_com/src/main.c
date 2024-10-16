#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include <time.h>

#include "IO.h"
#include "msg_com.h"

const key_t MSG_ID = 150;

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error ");
        return 1;
    }

    // Parent proccess (writer)
    if (pid) {
        Buffer buffer = {};
        buffer_ctor(&buffer, BUFFER_CAPACITY);

        int msg_d = msgget(MSG_ID, IPC_CREAT | 0666);
        if (msg_d == -1) {
            perror("parent msgget error ");
            return 1;
        }
        
        printf("MSG_D: %d\n", msg_d);

        msg sbuf = {.mtype = MSG_TYPE, .mtext = {}};

        int input_d = open_input_file(INPUT_FILENAME);
        if (input_d == -1) return 1;

        int msg_len = 0;
        while ((msg_len = read_from_file(&buffer, input_d))) {
            if (msg_len == -1) {
                perror("read error ");
                return 1;
            }

            size_t current_position = 0;
            while (current_position + MESSAGE_CAPACITY < buffer.size) {
                memcpy(sbuf.mtext, buffer.buffer + current_position, MESSAGE_CAPACITY);

                current_position += MESSAGE_CAPACITY;

                if((msgsnd(msg_d, (void *) &sbuf, MESSAGE_CAPACITY, 0))) {
                    perror("msgsnd error ");
                    return 1;
                }
            }

            memcpy(sbuf.mtext, buffer.buffer + current_position, buffer.size - current_position);
            if((msgsnd(msg_d, (void *) &sbuf, buffer.size - current_position, 0))) {
                perror("msgsnd error ");
                return 1;
            }
        }

        if((msgsnd(msg_d, (void *) &sbuf, 0, 0))) {
            perror("msgsnd error ");
            return 1;
        }

        close(input_d);
        buffer_dtor(&buffer);

        int status = 0;
        waitpid(pid, &status, 0);
    }
    // Child process (receiver)
    else {
        int msg_d = msgget(MSG_ID, 0666);
        if (msg_d == -1) {
            perror("child msgget error ");
            return 1;
        }

        printf("MSG_D: %d\n", msg_d);

        msg rbuf = {};

        int output_d = crate_output_file(OUTPUT_FILENAME);
        if (output_d == -1) return 1;

        clock_t proc_time = clock();

        int message_len = 0;
        while ((message_len = msgrcv(msg_d, &rbuf, MESSAGE_CAPACITY, MSG_TYPE, 0))) {
            if (message_len == -1) {
                perror("msgrcv error ");
                return 1;
            }

            if (write(output_d, rbuf.mtext, message_len) < 0) {
                perror("write error ");
                return 1;
            }
        }

        proc_time = clock() - proc_time;
        printf("Receive time: %ld\n", proc_time);
        
        close(output_d);
        struct msqid_ds qstatus;
        msgctl(msg_d, IPC_RMID, &qstatus);
    }

    return 0;
}