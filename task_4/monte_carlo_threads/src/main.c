#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "config.h"
#include "function.h"
#include "monte_carlo.h"

int main() {

    Integral integral = {
                        .start    = 2.0,
                        .end      = 3.0,
                        .function = function,
                        .result   = 0.0 };

    double len = integral.end - integral.start;
    double div = len / THREADS_COUNT;

    pthread_t threads[THREADS_COUNT]       = {};
    Integral  sub_integrals[THREADS_COUNT] = {};

    for (size_t i = 0; i < THREADS_COUNT; i++) {
        sub_integrals[i].start    = integral.start + div * i;
        sub_integrals[i].end      = sub_integrals[i].start + div;
        sub_integrals[i].function = integral.function;

        pthread_create(&(threads[i]), NULL, monte_carlo, &(sub_integrals[i]));
    }

    for (size_t i = 0; i < THREADS_COUNT; i++) {// BUG slow
        pthread_join(threads[i], NULL);

        integral.result += sub_integrals[i].result;
    }

    printf("Final: %lf\n", integral.result);

    return 0;
}