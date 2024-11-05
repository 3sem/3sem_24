#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>

#include "config.h"
#include "message_queue.h"

int exchangeMessageQueue(int fd_read, int fd_write) {
    int msgid = msgget(MY_FTOK_KEY + 333, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Error msgget failed");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        Message msg_cons;
        
        for (ssize_t len_msg = 0; 1; ) {
            len_msg = msgrcv(msgid, &msg_cons, sizeof(msg_cons.mtext), 0, 0);

            if (msg_cons.mtype != DONE_TYPE_MESSAGE) {
                write(fd_write, msg_cons.mtext, len_msg);
            } else {
                break;
            }
        }
        exit(EXIT_SUCCESS);
    } else {
        Message msg_prod;

        for (int len_msg = 0; 1; ) {
            len_msg = read(fd_read, msg_prod.mtext, BUF_SIZE);
                            
            if (len_msg) {
                msg_prod.mtype = STD_TYPE_MESSAGE;
                if (msgsnd(msgid, &msg_prod, len_msg, 0) == -1) {
                    perror("Error msgsnd failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                msg_prod.mtype = DONE_TYPE_MESSAGE;
                msgsnd(msgid, &msg_prod, 0, 0);
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

    msgctl(msgid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
