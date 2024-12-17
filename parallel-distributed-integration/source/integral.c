#include "integral.h"
#include "log_utils.h"

#include <time.h>
#include <pthread.h>
#include <unistd.h>

void* thread_worker(void *arg) 
{
    ThreadTask *task = (ThreadTask *)arg;
    double sum = 0.0;

    for (int i = 0; i < task->points; i++) 
    {
        double x = task->start + (task->end - task->start) * (rand_r(&task->seed) / (double)RAND_MAX);
        sum += x * x;
    }

    task->result = (task->end - task->start) * sum / task->points;

    pthread_exit(NULL);
}

void monte_carlo_integration(Task *task, double *result) 
{
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

    LOG("[Server] Calculating integral in threads from %.2f to %.2f with %zu points\n",
           task->start, task->end, task->points);

    pthread_t threads[num_cores];
    ThreadTask thread_tasks[num_cores];

    int points_per_thread = task->points / num_cores;
    double range_per_thread = (task->end - task->start) / num_cores;

    for (int i = 0; i < num_cores; i++) 
    {
        thread_tasks[i].start = task->start + i * range_per_thread;
        thread_tasks[i].end = task->start + (i + 1) * range_per_thread;
        thread_tasks[i].points = points_per_thread;
        thread_tasks[i].seed = (unsigned int)time(NULL) ^ pthread_self() ^ i;

        pthread_create(&threads[i], NULL, thread_worker, &thread_tasks[i]);
    }

    *result = 0.0;

    for (int i = 0; i < num_cores; i++) 
    {
        pthread_join(threads[i], NULL);
        *result += thread_tasks[i].result;
    }

    LOG("[Server] Total result: %.10f\n", *result);
}


