#include "integrator.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include <pthread.h>

const size_t N = 1000000000;
pthread_mutex_t mutex;

int cmp_double(double first_double, double second_double)
{
    const double eps = 1e-7;

    if(isnan(first_double) && isnan(second_double))
    {
        return 0;
    }
    if (fabs(first_double - second_double) < eps)
    {
        return 0;
    }
    else if ((first_double - second_double) > eps)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

double generate_rand(Interval interval, unsigned int *seed)
{
    double scale = rand_r(seed) / (double)RAND_MAX;
    double result = interval.a + scale * (interval.b - interval.a);

    return result;
}

void* baby_gronk(void* arg)
{
    Baby_Gronk_Args* args = (Baby_Gronk_Args*)arg;
    size_t k = 0;

    for (size_t id = 0; id < N; ++id)
    {
        double x = generate_rand(args->x_interval, &(args->seed));
        Interval y_interval = {.a = 0, .b = args->max_y};
        double y = generate_rand(y_interval, &(args->seed));

        if (cmp_double(y, args->f(x)) <= 0)
        {
            ++k;
        }
    }

    double partial_result = (args->x_interval.b - args->x_interval.a) * args->max_y * k / N;

    pthread_mutex_lock(&mutex);
    *(args->result) += partial_result;
    pthread_mutex_unlock(&mutex);

    free(args);
    return NULL;
}

double integrate(double (*f)(double), Interval x_interval)
{
    pthread_mutex_init(&mutex, NULL);

    unsigned int seed = (unsigned)time(NULL);

    double max_y = 0.0;

    for (size_t id = 0; id < N; ++id)
    {
        double x = generate_rand(x_interval, &seed);
        double y = f(x);

        if (y > max_y) max_y = y;
    }

    #ifdef ENABLE_LOGGING
    printf("max y: %lf\n", max_y);
    #endif

    const size_t thread_amount = 1;
    double step = (x_interval.b - x_interval.a) / thread_amount;
    double result = 0;

    pthread_t threads[thread_amount];

    for (size_t id = 0; id < thread_amount; ++id)
    {
        Interval inter_part =
		{
            .a = x_interval.a + step * id,
            .b = x_interval.a + step * (id + 1)
        };

        Baby_Gronk_Args* args = malloc(sizeof(Baby_Gronk_Args));
        if (args == NULL)
        {
            perror("Failed to allocate memory for thread arguments");
            exit(EXIT_FAILURE);
        }

       seed = (unsigned)time(NULL) + (unsigned)id;

		*args = (Baby_Gronk_Args)
		{
			.f = f,
			.max_y = max_y,
			.result = &result,
			.x_interval = inter_part,
			.seed = seed,
		};

        pthread_create(&threads[id], NULL, baby_gronk, args);
    }

    for (size_t id = 0; id < thread_amount; ++id)
    {
        pthread_join(threads[id], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return result;
}
