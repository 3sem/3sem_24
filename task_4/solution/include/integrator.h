#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <stdlib.h>

typedef struct
{
    size_t thread_id;
    size_t points;
    double* areas;
    double segment_start;
    double segment_end;
	double (*f)(double);
} ThreadData;

double integrate(	double (*f)(double),
					double begin,
					double end,
					size_t thread_amount,
					size_t point_amount);

void* calculate_fragment(void* arg);

#endif // INTEGRATOR_H
