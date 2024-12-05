#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "integral.h"

key_t generateKey() {
    char tmpFile[] = "/tmp/uniqueOPIOP";
    int fileDescriptor = mkstemp(tmpFile);
    if (fileDescriptor == -1) return -1;
    key_t key = ftok(tmpFile, 'R');
    close(fileDescriptor);
    unlink(tmpFile);
    return key;
}

struct MemoryBuffer* initializeSharedSegment(int* shmID) {
    key_t key = generateKey();
    *shmID = shmget(key, SHM_SIZE, SEGMENT_PERMISSIONS | IPC_CREAT);
    if (*shmID < 0) return NULL;

    struct MemoryBuffer* sharedMem = shmat(*shmID, NULL, 0);
    if (sharedMem == (void*)-1) return NULL;

    sharedMem->resultAccumulator = 0;
    sem_init(&sharedMem->accessSemaphore, 1, 1);
    return sharedMem;
}

ssize_t monteCarloEvaluation(double start, double end, double range, double (*func)(double), size_t numSamples) {
    size_t positiveHits = 0, negativeHits = 0;
    unsigned int seed = (unsigned int)time(NULL);

    for (size_t i = 0; i < numSamples; ++i) {
        double randomX = start + (rand_r(&seed) / (double)RAND_MAX) * (end - start);
        double randomY = (rand_r(&seed) / (double)RAND_MAX) * range;
        double functionValue = func(randomX);

        if (functionValue > 0 && randomY < functionValue) positiveHits++;
        if (functionValue < 0 && randomY < -functionValue) negativeHits++;
    }
    return (ssize_t)(positiveHits - negativeHits);
}

double GetHeight(double start, double end, double (*func)(double)) {
    double MaxValue = func(end); double MinValue = func(start);
    int accuracy = 1000000;
    double delta = (end - start) / accuracy;
    double curX; double curY;
    for (int i = 0; i < accuracy; i++) {
        curX = start + delta * i; curY = func(curX);
        if (curY > MaxValue) MaxValue = curY;
        if (curY < MinValue) MinValue = curY;
    }
    
    if (MaxValue > 0 && MaxValue > -MinValue) return MaxValue;
    return -MinValue;
}

void executeParallelCalculation(double a, double b, double (*func)(double), int processes, struct MemoryBuffer* sharedMem) {
    double segmentLength = (b - a) / processes;
    double height = GetHeight(a, b, func);
    int CurScale = MAX_RANDOM_SCALE / processes;

    for (int i = 0; i < processes; ++i) {
        if (fork() == 0) {
            double localStart = a + i * segmentLength;
            double localEnd = localStart + segmentLength;

            ssize_t localResult = monteCarloEvaluation(localStart, localEnd, height, func, CurScale);
            double localIntegral = height * segmentLength * (double)localResult / CurScale;

            sem_wait(&sharedMem->accessSemaphore);
            sharedMem->resultAccumulator += localIntegral;
            sem_post(&sharedMem->accessSemaphore);

            shmdt(sharedMem);
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < processes; ++i) {
        int status;
        wait(NULL);
    }
}

double computeIntegral(double a, double b, double (*func)(double), int processCount) {
    int sharedMemoryID;
    struct MemoryBuffer* sharedMem = initializeSharedSegment(&sharedMemoryID);
    if (!sharedMem) { perror("Shared memory initialization failed"); return NAN;}

    pid_t parentProcess = fork();
    if (parentProcess == 0) {
        executeParallelCalculation(a, b, func, processCount, sharedMem);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        double finalResult = sharedMem->resultAccumulator;
        shmdt(sharedMem);
        shmctl(sharedMemoryID, IPC_RMID, NULL);
        return finalResult;
    }
}