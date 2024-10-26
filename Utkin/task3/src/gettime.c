#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "gettime.h"
#include "fifo.h"
#include "queue.h"
#include "sharedmem.h"

int timeShMem(int fread, int fwrite, struct timespec* start, struct timespec* end) {
    ShMemData* data = Init_shdata();
    clock_gettime(CLOCK_MONOTONIC, start);
    int res = SharedMemory(fread, fwrite, data);
    clock_gettime(CLOCK_MONOTONIC, end);
    DeleteShdata(data);
    return res;
}

int timeFIFO(int fread, int fwrite, struct timespec* start, struct timespec* end) {
    Init_Fifo();
    clock_gettime(CLOCK_MONOTONIC, start);
    int res = FIFO(fread, fwrite);
    clock_gettime(CLOCK_MONOTONIC, end);
    return res;
}

int timeQueue(int fread, int fwrite, struct timespec* start, struct timespec* end) {
    Queue* queue = malloc(sizeof(Queue));
    int msgid = Init_queue();
    clock_gettime(CLOCK_MONOTONIC, start);
    int res = sendQueue(fread, fwrite, queue, msgid);
    clock_gettime(CLOCK_MONOTONIC, end);
    return res;
}

double getTIME(int(*filetransfer)(int, int, struct timespec*, struct timespec*)) {
    struct timespec start, end;
    double restime = 0;
    int fread = open("text1", O_RDONLY);
    int fwrite = open("text2", O_WRONLY | O_CREAT, 0666);
    if (fread == -1) perror("File1 dont open");
    if (fwrite == -1) perror("File2 dont open");

    filetransfer(fread, fwrite, &start, &end);

    restime += (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    close(fread); close(fwrite);
    remove("text2");

    return restime;
}