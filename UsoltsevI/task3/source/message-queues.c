//-----------------------------------------------------------------
//
// Message queues file translation implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/message-queues.h"
#include "../include/define-check-condition-ret.h"
#include "../include/buf-size.h"
#include "../config/config.h"

#if FILESENDING_BUF_SIZE < 8192
    static const size_t MSGQ_BUF_SIZE       = FILESENDING_BUF_SIZE;
#else 
    static const size_t MSGQ_BUF_SIZE       = 8192;
#endif

typedef struct msgbuf {
    long   mtype;
    char*  mtext;
    size_t bsize;
} msgbuf_t;

static msgbuf_t *construct_msgbuf(long mtype, size_t bsize) {
    msgbuf_t *buf = (msgbuf_t *) malloc(sizeof(msgbuf_t));
    CHECK_CONDITION_PERROR_RET(buf == NULL, "malloc", NULL);
    buf->bsize  = bsize;
    buf->mtype  = mtype;
    buf->mtext  = (char *) calloc(bsize, sizeof(char));
    CHECK_CONDITION_FREE_RET(buf->mtext == NULL, buf, NULL, NULL);
    return buf;
}

static void delete_msgbuf(msgbuf_t *buf) {
    if (buf != NULL) {
        free(buf->mtext);
        free(buf);
    }
}

int msgq_translate_file(int fd, size_t file_size) {
    key_t key  = ftok(MSGQ_TEMP_FILE_NAME, 0);
    
    int msg_id = msgget(key, IPC_CREAT | 0666);
    CHECK_CONDITION_PERROR_RET(msg_id == -1, "msgget", 1);

    pid_t pid = fork();
    CHECK_CONDITION_PERROR_RET(pid < 0, "fork", 1);
    
    if (pid > 0) {
        int status;
        wait(&status);
        CHECK_CONDITION_RET(status != 0, 1);

        int fd = open(MSGQ_RCVD_FILE_NAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        CHECK_CONDITION_PERROR_RET(fd == -1, "open", 1);

        msgbuf_t *buf = construct_msgbuf(1, MSGQ_BUF_SIZE);

        size_t char_read = 0;
        size_t char_rcvd = 0;

        while ((char_read = msgrcv(msg_id, &buf, buf->bsize, 0, 0)) > 0) {
            size_t char_write = write(fd, buf->mtext, buf->bsize);
            CHECK_CONDITION_PRINT(char_write != buf->bsize);
            char_rcvd += char_read;
            if (char_rcvd >= file_size) {
                break;
            }
        }

        close(fd);
        delete_msgbuf(buf);

        int msgctl_res = msgctl(msg_id, IPC_RMID, NULL);
        CHECK_CONDITION_PERROR_RET(msgctl_res != 0, "msgctl", 1)

        if (char_rcvd >= file_size) {
            return 0;
        }

    } else {
        msgbuf_t *buf = construct_msgbuf(1, MSGQ_BUF_SIZE);

        if (buf == NULL) {
            fprintf(stderr, "construct_msgbuf");
            exit(1);
        }

        size_t char_read = 0;
        size_t char_sent = 0;

        while ((char_read = read(fd, buf->mtext, buf->bsize)) > 0) {
            int msgsnd_ret = msgsnd(msg_id, &buf, char_read, 0);
            CHECK_CONDITION_PERROR(msgsnd_ret != 0, "msgsnd")
            char_sent += char_read;
            if (char_sent >= file_size) {
                break;
            }
        }

        delete_msgbuf(buf);
        exit(0);
    }

    return 1;
}
