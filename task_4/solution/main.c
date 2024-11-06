#include "integrator.h"
#include <stdio.h>
#include <time.h>

double f(double x)
{
    return x * x + 2 * x + 3;
}

int main()
{
    Interval x_interval = {2, 5};

    struct timespec start, end;

    // Get the start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Call the integrate function
    double result = integrate(f, x_interval);

    // Get the end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate the time difference in seconds
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Result: %lf\n", result);
    printf("Execution time: %lf seconds\n", elapsed);

    return 0;
}
