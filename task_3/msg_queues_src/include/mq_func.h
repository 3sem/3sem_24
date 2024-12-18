#ifndef MSG_QUEUES
#define MSG_QUEUES

//#define _GHU_SOURCE

#include <unistd.h>
#include <sys/stat.h>
#include <sys/msg.h>

//#define MSG_SIZE 1020

#define KEY_NAME "./key_to_your_heart"

struct msgbuf
{
    long    mtype;
    char    msg[BUFF_SIZE];
};

typedef struct
{
    key_t           key;
    int             mq_id;
    struct msgbuf  *msgbuf;
    size_t          buf_size;
} mq_st;

int mq_st_create(mq_st *mq, const char *key_name);

void mq_st_destr(mq_st *mq);

int send_file_w_mq(mq_st *mq, const char *file_name);

int rcv_file_w_mq(const char *rcv_file_name, mq_st *mq);

int create_buff(mq_st *mq);


#endif