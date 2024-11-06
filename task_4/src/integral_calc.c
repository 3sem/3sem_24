#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include "integral_calc.h"

int set_points(point_t *points);

double point_belonging_check(point_t *points, double(alg_func)(double));


int set_points(point_t *points)
{
    double step = sqrt(pow(MAX_COORD, 2) / (double)POINTS_NUMBER);
    LOG("> step is: %lf\n", step);

    long long int i = 0;
    for (double y = 0; (y < MAX_COORD) && (i < POINTS_NUMBER); y += step)
    {
        for (double x = 0; (x < MAX_COORD) && (i < POINTS_NUMBER); x += step)
        {
            points[i].x = x;
            points[i].y = y;

            i++;  
        }
    }

    return 0;
    
}

double point_belonging_check(point_t *points, double(alg_func)(double))
{
    assert(points);

    double result = 0;
    long long int points_num = 0;
    for (int i = 0; i < POINTS_NUMBER; i++)
    {
        result = alg_func(points[i].x);
        if (result > MAX_COORD)
        {
            printf("Warning");
        }
        
        points_num += (result < points[i].y);
    }

    printf("points: %lld\n", points_num);

    return ((double)points_num / POINTS_NUMBER);
}

int calculate_integral(double(alg_func)(double))
{
    point_t *points = (point_t *)calloc(POINTS_NUMBER, sizeof(point_t));
    RETURN_ERROR_ON_TRUE(!points, 11, 
        printf("> points array memory allocation error\n"););
    
    char *belongs_bool = (char *)calloc(POINTS_NUMBER, sizeof(char));
    RETURN_ERROR_ON_TRUE(!belongs_bool, 12,
        perror("> memory allocation error\n"););

    set_points(points);
    double percent = point_belonging_check(points, alg_func);

    double integral = MAX_COORD * MAX_COORD * percent;
    printf("> result is: %lf\n", integral);

    free(points);
    free(belongs_bool);

    return 0;
    
}