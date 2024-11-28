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

#include "queue.h"
#include "argvs.h"
#include "sharedmem.h"

int Init_queue() {
    key_t key = generate_unique_key(111);
    if (key == (key_t)-1) perror("ftok failed");
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) perror("msgget failed");
    return msgid;
}

int sendQueue(int fread, int fwrite, Queue* queue, int msgid) {
    pid_t pid = fork();
    if (pid < 0) perror("Error fork failed");
    else if (pid == 0) {
        size_t len;
        while(1) {
            //printf("in rcv\n");
            len = msgrcv(msgid, queue, sizeof(queue->data), 0, 0);
            //printf("len: %d\ntype: %d\n", len, queue->type);
            if (queue->type == 1) write(fwrite, queue->data, len);
            else {
                //printf("go into break"); 
                break;}
        }
        //printf("exit child while\n");
        exit(EXIT_SUCCESS);
    } else {
        size_t len;
        while (1) {
            len = read(fread, queue->data, BUF_SIZE);
            if (len == -1) perror("Error read failed");
            if (len) {
                queue->type = 1;
                if (msgsnd(msgid, queue, len, 0) == -1) {
                    //printf("len: %d\nmsgid: %d\n", len, msgid);
                    perror("Error msgsnd failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                //printf("Send type = 2\n");
                queue->type = 2;
                msgsnd(msgid, queue, 0, 0);
                break;
            }
        }
        //printf("exit parent while\n");
    }

    int status = EXIT_FAILURE;
    wait(&status);
    if (status != EXIT_SUCCESS) perror("Error wait failed");
    msgctl(msgid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}

int QueueSendFile(int fread, int fwrite) {
    Queue* queue = malloc(sizeof(Queue));
    int msgid = Init_queue();
    int res = sendQueue(fread, fwrite, queue, msgid);
    return res;
}
