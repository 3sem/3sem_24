//-----------------------------------------------------------------
//
// Math Functions implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../include/functions.h"

math_function get_math_function_string(const char *name) {
    if (name == NULL) {
        return NULL;
    }

    // I don't wont to use a hash map...

    if (!strcmp(name, "x")) {
        return &function_x;
    }

    if (!strcmp(name, "x2")) {
        return &function_x2;
    }

    if (!strcmp(name, "x3")) {
        return &function_x3;
    }

    if (!strcmp(name, "sin")) {
        return &function_sin;
    }

    if (!strcmp(name, "cos")) {
        return &function_cos;
    }

    return NULL;
}

double function_x (double x) {
    return x;
}

double function_x2(double x) {
    return x * x;
}

double function_x3(double x) {
    return x * x * x;
}

double function_sin(double x) {
    return sin(x);
}

double function_cos(double x) {
    return cos(x);
}
