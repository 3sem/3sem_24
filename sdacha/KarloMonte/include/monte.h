#ifndef MONTE_H
#define MONTE_H


#include <stdint.h>


double calc_integral( double (*const func)( const double x), 
			          const double x_min, 
                      const double x_max, 
            		  const uint32_t n_threads);


#endif // MONTE_H
