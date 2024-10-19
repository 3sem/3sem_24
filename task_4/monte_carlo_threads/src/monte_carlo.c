#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "config.h"
#include "monte_carlo.h"

void* monte_carlo(void* arg) {
    if (!arg) return NULL;

    Integral* integral = (Integral*) arg;   // Cast arg to integral type

    int points = POINTS_COUNT / THREADS_COUNT;
    int K = 0;

    double square = (integral->end - integral->start) * (integral->function(integral->end) - 0);
    double x = 0, y = 0;

    srand((unsigned int) time(NULL));

    double max_val = integral->function(integral->end);
    double len     = (integral->end - integral->start);
    unsigned int seed = 0;

    for (int i = 0; i < points; i++) {
        x = integral->start + (double) rand_r(&seed) / (double) RAND_MAX * len;
        y = (double) (rand_r(&seed)) / (double) RAND_MAX * max_val;

        if (fabs(y) - integral->function(x) < 0) K++;
    }

    integral->result = (square / points) * K;

    return integral;
}