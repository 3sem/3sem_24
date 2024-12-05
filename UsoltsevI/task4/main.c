//-----------------------------------------------------------------
//
// Monte Calro alghorithm main file
//
//-----------------------------------------------------------------
//
// Input param: <num_threads> <num_points> <math_munc>
// Example: 4 1000 x3
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/monte-carlo.h"
#include "include/functions.h"

static void measure_time(math_function func
                            , size_t num_points
                            , int num_threads) {
    double srt = 0.0;
    double end = 1.0;
    double min = 0.0;
    double max = 1.0;

    struct timespec start, stop;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    double s = concurrent_monte_carlo(func, srt, end, min, max, num_points, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    double execution_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1000000000;

    printf("square: %lf\n", s);
    printf("execution_time: %lf\n", execution_time);

}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("see usage instruction:\n");
        printf("Input param: <num_threads> <num_points> <math_munc>\n");
        return 0;
    }

    char *verifier;
    int num_threads = strtol(argv[1], &verifier, 10);
    if (verifier == argv[1]) {
        fprintf(stderr, "invalid number of threads\n");
        return 0;
    }

    size_t num_points  = strtol(argv[2], &verifier, 10);
    if (verifier == argv[2]) {
        fprintf(stderr, "invalid number of points\n");
        return 0;
    }

    math_function func = get_math_function_string(argv[3]);
    if (func == NULL) {
        fprintf(stderr, "invalid name of function\n");
        return 0;
    }

    measure_time(func, num_points, num_threads);

    return 0;
}
