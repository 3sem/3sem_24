#pragma once
//-----------------------------------------------------------------
//
// Math Functions header
//
//-----------------------------------------------------------------

typedef double (*math_function) (double);

math_function get_math_function_string(const char *name);

double function_x (double x);
double function_x2(double x);
double function_x3(double x);
double function_sin(double x);
double function_cos(double x);
