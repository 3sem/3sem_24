#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <stdlib.h>

double generate_rand(double a, double b);
double integrate(double (*f) (double), double a, double b);

#endif // INTEGRATOR_H
