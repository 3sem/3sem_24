//#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include "common_func.h"
#include "mq_func.h"

int mq_st_create(mq_st *mq, const char *key_name)
{
    assert(mq);
    assert(key_name);

    key_t key = ftok(key_name, 1);
    RETURN_ERROR_ON_TRUE(key == -1, -1, 
        perror("key creation fail"););

    int msgq_id = msgget(key, IPC_CREAT | S_IRWXU);
    RETURN_ERROR_ON_TRUE(msgq_id == -1, -1, 
        perror("message queue creation fail"););

    struct msgbuf *buff = (struct msgbuf *)calloc(1, sizeof(struct msgbuf));
    RETURN_ERROR_ON_TRUE(!buff, -1,
        printf("message buff creation error\n");
        msgctl(msgq_id, IPC_RMID, NULL););

    mq->key         = key;
    mq->buf_size    = BUFF_SIZE;
    mq->mq_id       = msgq_id;
    mq->msgbuf      = buff;

    return 0;
}

/*int create_buff(mq_st *mq)
{
    assert(mq);

    struct msgbuf *msgbuf = (struct msgbuf *)calloc(1, sizeof(struct msgbuf));
    RETURN_ERROR_ON_TRUE(!msgbuf, -1, 
        printf("structure memory allocation error\n"););
    msgbuf->mtype   = 1;
    msgbuf->msg = (char *)calloc(mq->buf_size, sizeof(char));
    RETURN_ERROR_ON_TRUE(!msgbuf->msg.msg, -1,
        printf("> buffer memory allocation error\n"););

    LOG("structure is:\n1) %ld\n2)%p\n", msgbuf->mtype, msgbuf->msg.msg);

    mq->msgbuf = msgbuf;

    return 0;
}*/

void mq_st_destr(mq_st *mq)
{
    assert(mq);

    msgctl(mq->mq_id, IPC_RMID, NULL);
    free(mq->msgbuf);

    mq->key      = 0;
    mq->buf_size = 0;
    mq->mq_id    = 0;
    mq->msgbuf   = NULL;

    return;
}

int send_file_w_mq(mq_st *mq, const char *file_name)
{
    assert(mq);
    assert(file_name);

    off_t   file_size   = 0;
    ssize_t err_num     = 0;
    
    int fd = open(file_name, O_RDONLY);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("file opening error"););

    file_size = get_file_size(fd);
    RETURN_ERROR_ON_TRUE(file_size == -1, -1);

    size_t bytes_left       = (size_t)file_size;
    memcpy(mq->msgbuf->msg, &file_size, sizeof(size_t));
    mq->msgbuf->mtype = 1;

    LOG("> size: %lu\n", *(size_t *)mq->msgbuf->msg);

    err_num = msgsnd(mq->mq_id, mq->msgbuf, sizeof(size_t), 0);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1, 
        perror("size send error"););

    while (bytes_left != 0)
    {
        size_t bytes_to_send = bytes_left > mq->buf_size ? mq->buf_size : bytes_left;

        LOG("> bytes to send: %lu\n", bytes_to_send);

        err_num = read(fd, mq->msgbuf->msg, bytes_to_send);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("read from file error");
            close(fd););

        LOG("> data sending: %s\n", mq->msgbuf->msg);

        err_num = msgsnd(mq->mq_id, mq->msgbuf, bytes_to_send, 0);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("data send error"););

        bytes_left -= bytes_to_send;
    }
    
    close(fd);

    return 0;
}

int rcv_file_w_mq(const char *rcv_file_name, mq_st *mq)
{
    assert(rcv_file_name);
    assert(mq);

    LOG("> buf addres: %p\n", mq->msgbuf);

    int fd = open(rcv_file_name, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("rcv file opening error"););

    size_t  bytes_left  = 0;
    ssize_t err_num     = 0;

    err_num = msgrcv(mq->mq_id, mq->msgbuf, sizeof(size_t), 0, 0);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("size recieve error"););
    memcpy(&bytes_left, mq->msgbuf->msg, sizeof(size_t));

    LOG("> data left to read is: %lu\n", bytes_left);

    while (bytes_left != 0)
    {
        size_t bytes_to_read = bytes_left > mq->buf_size ? mq->buf_size : bytes_left;

        LOG("> bytes need to read: %lu\n", bytes_left);

        err_num = msgrcv(mq->mq_id, mq->msgbuf, bytes_to_read, 0, 0);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("message recieve error"););

        LOG("bytes read: %ld\n", err_num);
        LOG("> data read: %s\n", mq->msgbuf->msg);

        err_num = write(fd, mq->msgbuf->msg, bytes_to_read);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("write into fd error");
            close(fd););

        bytes_left -= bytes_to_read;
    }

    close(fd);

    return fd;
}

