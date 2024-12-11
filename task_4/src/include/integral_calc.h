#ifndef INTEG_CALC
#define INTEG_CALC

typedef struct 
{
    double x;
    double y;
} point_t;

typedef struct
{
    double              (* math_func)(double x);
    int                 thread_number;
    point_t             *points_arr;
    long int            *res_ptr;
} integr_func_args;

#define POINTS_NUMBER       10000128
#define X_LIMIT             10
#define Y_LIMIT             100

#define NUMBER_OF_THREADS   128

#define X_RANGE             (double)((double)X_LIMIT / (double)NUMBER_OF_THREADS)
#define POINTS_PER_THREAD   (int)(POINTS_NUMBER / NUMBER_OF_THREADS)

#define RETURN_ERROR_ON_TRUE(statement, return_val, ...)                \
    do                                                                  \
    {                                                                   \
        if (statement)                                                  \
        {                                                               \
            __VA_ARGS__                                                 \
            return return_val;                                          \
        }                                                               \
    } while (0)

#define LOG(...)    //fprintf(stderr, __VA_ARGS__)

int calculate_integral(double(math_func)(double));

#endif