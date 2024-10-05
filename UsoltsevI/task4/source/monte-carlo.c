//-----------------------------------------------------------------
//
// Monte Calro alghorithm implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/monte-carlo.h"
#include "../include/functions.h"

static double max = 1.0;
static double min = 1.0;

static struct monte_carlo_args {
    math_function func;
    double srt;
    double end;
    double min;
    double max;
    double result;
    size_t num_points;
};

static struct monte_carlo_args *args_create(double func(double x)
                        , double srt
                        , double end
                        , double min
                        , double max
                        , size_t num_points) {
    struct monte_carlo_args *args = malloc(sizeof(struct monte_carlo_args));

    if (args == NULL) {
        perror("malloc");
        return NULL;
    }

    args->func = func;
    args->srt  = srt;
    args->end  = end;
    args->min  = min;
    args->max  = max;
    args->result = 0.0;
    args->num_points = num_points;

    return args;
}

static void *monte_carlo(void *v_args) {
    struct monte_carlo_args *args = (struct monte_carlo_args *) v_args;

    if (args->min >= args->max || args->srt >= args->end) {
        return args;
    }

    double x;
    double y;
    size_t k = 0;
    unsigned int seed;

    srand(time(NULL));

    for (size_t i = 0; i < args->num_points; ++i) {
        x = (double) rand_r(&seed) / RAND_MAX * (args->end - args->srt) + args->srt;
        y = (double) rand_r(&seed) / RAND_MAX * (args->max - args->min) + args->min;

        if (y - args->func(x) < 0) {
            k += 1;
        }
    }

    args->result = (args->max - args->min) * (args->end - args->srt) * k / args->num_points;

    return args;
}

double concurrent_monte_carlo(double func(double x)
                    , double srt
                    , double end
                    , double min
                    , double max
                    , size_t num_points
                    , size_t num_threads) {
    pthread_t *threads = (pthread_t *) calloc(num_threads, sizeof(pthread_t));

    double step = (end - srt) / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        struct monte_carlo_args *args = args_create(func, srt + step * i, end - step * (num_threads - i - 1), min, max, num_points);
        pthread_create(&threads[i], NULL, monte_carlo, args);
    }

    double square = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        struct monte_carlo_args *result;
        pthread_join(threads[i], &result);
        square += result->result;
        free(result);
    }

    free(threads);

    return square;
}
