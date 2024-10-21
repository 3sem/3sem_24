#ifndef INTEG_CALC
#define INTEG_CALC

typedef struct 
{
    double x;
    double y;
} point_t;

#define POINTS_NUMBER   100000000
#define MAX_COORD       100

#define RETURN_ERROR_ON_TRUE(statement, return_val, ...)                \
    do                                                                  \
    {                                                                   \
        if (statement)                                                  \
        {                                                               \
            __VA_ARGS__                                                 \
            return return_val;                                          \
        }                                                               \
    } while (0)

#define LOG(...) fprintf(stderr, __VA_ARGS__);

int calculate_integral();

#endif