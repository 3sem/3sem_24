#pragma once
//-----------------------------------------------------------------
//
// Monte Calro alghorithm header
//
//-----------------------------------------------------------------

#include <stdio.h>

double concurrent_monte_carlo(double func(double x)
                    , double srt
                    , double end
                    , double min
                    , double max
                    , size_t num_points
                    , size_t num_threads);
