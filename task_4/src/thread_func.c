#include <pthread.h>
#include <stdio.h>
#include <assert.h>
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
    //заменить на случайные числа
    double x_step = X_LIMIT / sqrt(POINTS_NUMBER);
    double y_step = Y_LIMIT / sqrt(POINTS_NUMBER);
    LOG("> x step is: %lf\n", x_step);
    LOG("> y step is: %lf\n", y_step);

    long long int i = 0;
    for (double x = 0; (x < X_RANGE) && (i < POINTS_PER_THREAD); x += x_step)
    {
        for (double y = 0; (y < Y_LIMIT) && (i < POINTS_PER_THREAD); y += y_step)
        {
            args->points_arr[i].x = args->thread_number * X_RANGE + x;
            args->points_arr[i].y = y;

            i++;  
        }
    }

    return;
}