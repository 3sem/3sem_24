#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#define POINTS_COUNT 1000000000

typedef struct _Int {
    double start, end;          // Integral limits
    double (*function)(double); // Function
    double result;              // Result
} Integral;

void* monte_carlo(void* arg);

#endif