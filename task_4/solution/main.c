#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "integrator.h"

const double begin = 0.0;
const double end = 1.0;

double f(double x)
{
    return x * x;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("Please enter number of threads and amount of segments\n");
        return 1;
    }

    size_t points_amount = (size_t)atoi(argv[1]);
    size_t thread_amount = (size_t)atoi(argv[2]);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    double result = integrate(f, begin, end, thread_amount, points_amount);

    gettimeofday(&end_time, NULL);

    double execution_time = (end_time.tv_sec - start_time.tv_sec) +
                            (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

	// printf("Result: %lf\n", result);
    // printf("Execution time: %f seconds\n", execution_time);
	printf("%lu %lf\n", thread_amount, execution_time);

    return 0;
}
