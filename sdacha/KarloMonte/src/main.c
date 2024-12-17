#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "monte.h"


static double 
func( const double x)
{
	// return exp( sqrt( sqrt( x)));
	return x * x;
}


int main( const int argc, const char *argv[])
{
	if ( argc != 2 )
	{
		return 1;
	}


	const double   min       = 1;
	const double   max       = 100000;
	const uint32_t n_threads = atoi( argv[1]);
	
	const double integral = calc_integral( func, min, max, n_threads);

	if ( integral == -1)
	{
		return 1;
	}

	printf( "Integral: %lg\n", integral);

	
	return 0;
}
