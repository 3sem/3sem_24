#include <stdio.h>
#include <math.h>
#include "integral_calc.h"

double square(double x);

double square(double x)
{
    return x;
}

int main(int argc, char const *argv[])
{
    calculate_integral(square);
    
    return 0;
}
