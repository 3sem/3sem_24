#include <bits/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "integral.h"


double f1(double x) { return sin(x); }

int main(void) {
    double a = 0;
    double b = 10;


    double res1 = computeDefiniteIntegral(a, b, f1);
    if (isnan(res1)) {
        perror("Error computeDefiniteIntegral");
        exit(EXIT_FAILURE);
    }
    printf("Integral: %g\n\n", res1);

    struct timespec start, end;
    double res_time = 0.0;

    int max_nstreams = 20;
    int nstarts = 10;
    for (int i = 1; i <= max_nstreams; ++i) {
        for (int j = 0; j < nstarts; ++j) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            testComputeDefiniteIntegral(a, b, f1, i);
            clock_gettime(CLOCK_MONOTONIC, &end);
            res_time += (double)(end.tv_sec - start.tv_sec) * 1e9 + (double)(end.tv_nsec - start.tv_nsec);
        }

        res_time /= nstarts;
        printf("For %3d streams time: %g мс\n", i, res_time / 1e6);
        res_time = 0.0;
    }

    return 0;
}
