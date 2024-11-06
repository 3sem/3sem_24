#include "integrator.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const size_t N = 1000000000;

int cmp_double(double first_double, double second_double)
{
    const double eps = 1e-7;

    if(isnan(first_double) && isnan(second_double))
    {
        return 0;
    }
    if (fabs(first_double - second_double) < eps)
    {
        return 0;
    }
    else if ((first_double - second_double) > eps)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

double generate_rand(double a, double b)
{
	double scale = rand() / (double) RAND_MAX;
	double result = a + scale * (b - a);

	return result;
}

double integrate(double (*f) (double), double a, double b)
{
	srand(time(NULL));

	double max_y = 0.0;

	for (size_t id = 0; id < N; ++id)
	{
		double x = generate_rand(a, b);
		double y = f(x);

		if (y > max_y) max_y = y;
	}

	printf("max y: %lf\n", max_y);

	size_t k = 0;

	for (size_t id = 0; id < N; ++id)
	{
		double x = generate_rand(a, b);
		double y = generate_rand(0, max_y);

		// printf("random pnt: (%.2lf, %.2lf)\n", x, y);

		if (cmp_double(y, f(x)) <= 0)
		{
			// printf("%lf <= %lf\n", y, f(x));
			++k;
		}
	}

	double result = (b - a) * max_y * k / N;
	return result;
}
