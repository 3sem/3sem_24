#include "integrator.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

double integrate(	double (*f)(double),
					double begin,
					double end,
					size_t thread_amount,
					size_t point_amount)
{
	size_t points_per_thread = point_amount / thread_amount;
    size_t rest_of_points = point_amount % thread_amount;

    double segment_width = (end - begin) / thread_amount;

	double* areas = (double*)calloc(thread_amount, sizeof(double));
	if (!areas)
	{
		perror("bad alloc");
		return 1;
	}

	pthread_t* threads = (pthread_t*) calloc(thread_amount, sizeof(pthread_t));
	if (!threads)
	{
		perror("bad alloc");
		return 0.0;
	}

	ThreadData* thread_data = (ThreadData*) calloc(thread_amount, sizeof(ThreadData));
	if (!thread_data)
	{
		perror("bad alloc");
		return 0.0;
	}

    for (size_t id = 0; id < thread_amount; id++)
	{
        thread_data[id].thread_id = id;
        thread_data[id].points = points_per_thread + (id < rest_of_points ? 1 : 0);

        thread_data[id].segment_start = begin + id * segment_width;
        thread_data[id].segment_end = begin + (id + 1) * segment_width;

        thread_data[id].areas = areas;
		thread_data[id].f = f;
        pthread_create(&threads[id], NULL, calculate_fragment, &thread_data[id]);
    }

    for (size_t id = 0; id < thread_amount; id++)
	{
        pthread_join(threads[id], NULL);
    }

    double total_integral = 0.0;
    for (size_t id = 0; id < thread_amount; id++)
	{
        total_integral += areas[id];
    }

	free(threads);
	free(thread_data);
	free(areas);

	return total_integral;
}

void* calculate_fragment(void* arg)
{
    ThreadData* data = (ThreadData*) arg;
    double sum = 0.0;
    unsigned int seed = (unsigned int)data->thread_id;

    for (size_t id = 0; id < data->points; id++)
	{
        double x = 	data->segment_start +
					(data->segment_end - data->segment_start) *
					rand_r(&seed) /
					(double)RAND_MAX;

        double y = data->f(x);
        sum += y;
    }

    data->areas[data->thread_id] = 	sum /
									data->points *
									(data->segment_end - data->segment_start);
    pthread_exit(NULL);
}
