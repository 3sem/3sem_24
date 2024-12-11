#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "integral_calc.h"
#include "thread_funcs.h"

long int point_belonging_check(integr_func_args *args);

void set_points(integr_func_args *args);

void *thread_integr_func(void *arg_ptr)
{
    assert(arg_ptr);

    integr_func_args *args = (integr_func_args *)arg_ptr;

    set_points(args);

    long int points_belongs = point_belonging_check(args);

    *(args->res_ptr) = points_belongs;

    return NULL;
}

long int point_belonging_check(integr_func_args *args)
{
    assert(args);

    double          value          = 0;
    long int        points_belongs  = 0;
    static char     warning         = 0;

    for (int i = 0; i < POINTS_PER_THREAD; i++)
    {
        value = args->math_func(args->points_arr[i].x);
        if (!warning && value > Y_LIMIT)
        {
            printf("[warning]> Function value is greater than Y limit, the integral will not be correct! Change the Y limit or put another function\n");
            warning = 1;
        }

        points_belongs += (args->points_arr[i].y < value);
    }

    LOG("points: %ld\n", points_belongs);

    return points_belongs;
}

void set_points(integr_func_args *args)
{
    assert(args);

    unsigned int seedp = RANDOM_SEQ_INT;

    LOG("points for thread: %d\n", POINTS_PER_THREAD);

    long long int i = 0;
    while (i < POINTS_PER_THREAD)
    {
        args->points_arr[i].x = (((double)rand_r(&seedp) / (double)RAND_MAX) + args->thread_number) * X_RANGE;
        args->points_arr[i].y = (double)rand_r(&seedp) / (double)RAND_MAX * Y_LIMIT;

        i++;
    }
    
    return;
}