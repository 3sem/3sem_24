#pragma once

#include <semaphore.h>
#include "argvs.h"

typedef struct {
    char data[BUF_SIZE];
    int len;
    int shmid;
    sem_t sem_wr;
    sem_t sem_rd;
} ShMemData;


key_t generate_unique_key(int a);
int DeleteShdata(ShMemData* shdata);
ShMemData* Init_shdata();
int SharedMemory(int fread, int fwrite, ShMemData* shdata);
int ShMemSendFile(int fread, int fwrite);