#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <math.h>
#include "integral_calc.h"
#include "send_results.h"
#include "thread_funcs.h"

int calculate_integral(double(math_func)(double))
{
    pthread_t           threads[NUMBER_OF_THREADS] = {};
    integr_func_args    args   [NUMBER_OF_THREADS] = {};

    long int    *results       = NULL;
    int         shm_fd         = 0;

    point_t *points = (point_t *)calloc(POINTS_NUMBER, sizeof(point_t));
    RETURN_ERROR_ON_TRUE(!points, -1, printf("> memory allocation for points error\n"););

    shm_fd = result_shm_arr_create(&results, NUMBER_OF_THREADS * sizeof(long int));
    RETURN_ERROR_ON_TRUE(shm_fd == -1, -1);

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        args[i].math_func           = math_func;
        args[i].points_arr          = points + (POINTS_PER_THREAD * i);
        args[i].res_ptr             = results + i;
        args[i].thread_number       = i;
    }

    int err_num = 0;
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        err_num = pthread_create(threads + i, 0, thread_integr_func, args + i);
        RETURN_ERROR_ON_TRUE(err_num, err_num,
            printf("thread creation error\n"););
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        RETURN_ERROR_ON_TRUE(pthread_join(threads[i], NULL), -1);
    }

    shm_close(results, NUMBER_OF_THREADS * sizeof(long int), shm_fd);
    free(points);

    return 0;
}