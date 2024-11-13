#include <stdio.h>
#include "integral_calc.h"
#include "timer.h"
#include "math_funcs.h"

int main()
{
    start_timer();
    calculate_integral(square);
    long long int time = stop_timer();
    printf("\033[0;33m> Time to calculate integral: %lld\033[0m\n", time);
    
    return 0;
}
