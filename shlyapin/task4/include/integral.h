#pragma once

double computeDefiniteIntegral(double a, double b, double (*f)(double));
double testComputeDefiniteIntegral(double a, double b, double (*f)(double), int nstreams);
