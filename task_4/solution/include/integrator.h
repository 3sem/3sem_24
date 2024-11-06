#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <stdlib.h>

typedef struct
{
	double a;
	double b;
} Interval;

typedef struct
{
    double (*f)(double);
    double max_y;
    double *result;
    Interval x_interval;
    unsigned int seed;  // Add seed field here
} Baby_Gronk_Args;

double generate_rand(Interval interval, unsigned int *seed);
double integrate(double (*f) (double), Interval x_interval);
void* baby_gronk(void* arg);

#endif // INTEGRATOR_H
