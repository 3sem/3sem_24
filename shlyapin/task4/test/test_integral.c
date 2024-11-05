#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <math.h>
#include <stdio.h>

#include "integral.h"
#include "config.h"

double f1(double x) { return sin(x); }
Test(Integral, computeDefiniteIntegral1) {
    double a = 0;
    double b = 10;

    double integral = computeDefiniteIntegral(a, b, f1);
    double expected = 1.8390715;

    cr_expect_float_eq(integral, expected, EPS);
}

double f2(double x) { return cos(x); }
Test(Integral, computeDefiniteIntegral2) {
    double a = 0;
    double b = 2 * M_PI;

    double integral = computeDefiniteIntegral(a, b, f2);
    double expected = 0;

    cr_expect_float_eq(integral, expected, EPS);
}

double f3(double x) { return x * x; }
Test(Integral, computeDefiniteIntegral3) {
    double a = 0;
    double b = 2;

    double integral = computeDefiniteIntegral(a, b, f3);
    double expected = 2.6666667;

    cr_expect_float_eq(integral, expected, EPS);
}

double f4(double x) { return x * x * x; }
Test(Integral, computeDefiniteIntegral4) {
    double a = -1;
    double b = 2;

    double integral = computeDefiniteIntegral(a, b, f4);
    double expected = 3.75;

    cr_expect_float_eq(integral, expected, EPS);
}

double f5(double x) { return x < 0 ? -x : x; }
Test(Integral, computeDefiniteIntegral5) {
    double a = -2;
    double b = 2;

    double integral = computeDefiniteIntegral(a, b, f5);
    double expected = 4;

    cr_expect_float_eq(integral, expected, EPS);
}

double f6(double x) { return exp(x); }
Test(Integral, computeDefiniteIntegral6) {
    double a = -40;
    double b = 0;

    double integral = computeDefiniteIntegral(a, b, f6);    
    double expected = 1;

    cr_expect_float_eq(integral, expected, EPS);
}
