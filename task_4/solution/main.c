#include "integrator.h"

#include <stdio.h>

double f(double x)
{
	return x * x + 2 * x + 3;
}

int main()
{
	double result = integrate(f, 2, 5);

	printf("%lf\n", result);

	return 0;
}
