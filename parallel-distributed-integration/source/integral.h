#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <stdlib.h>

struct _ThreadTask 
{
    double start;
    double end;
    int points;
    double result;
    unsigned int seed;
};

typedef struct _ThreadTask ThreadTask;

struct _Task
{
    double start;
    double end;

    size_t points;
};

typedef struct _Task Task;

void monte_carlo_integration(Task* task, double* result);

void* thread_worker(void *arg); 

#endif // !INTEGRAL_H
