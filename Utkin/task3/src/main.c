#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "fifo.h"
#include "queue.h"
#include "sharedmem.h"
#include "gettime.h"

int main() {
    double time = 0;
    for (int i = 0; i < NUMCYCL; i++) time += getTIME(timeShMem);
    printf("Shared memory: %f ms\n", time/1000000/NUMCYCL);
    if (BUF_SIZE <= 1024*8) {
        time = 0;
        for (int i = 0; i < NUMCYCL; i++) time += getTIME(timeQueue);
        printf("Queue: %f ms\n", time/1000000/NUMCYCL);
    }
    time = 0;
    for (int i = 0; i < NUMCYCL; i++) time += getTIME(timeFIFO);
    printf("FIFO: %f ms\n", time/1000000/NUMCYCL);
    return 0;
}