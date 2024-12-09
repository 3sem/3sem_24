#pragma once

#include "argvs.h"

typedef struct {
    long type;
    char data[BUF_SIZE];
} Queue;

int Init_queue();
int sendQueue(int fread, int fwrite, Queue* queue, int msgid);
int QueueSendFile(int fread, int fwrite);