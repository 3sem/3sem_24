#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#include <stdio.h>

#define LOG(...) fprintf(stderr, __VA_ARGS__);

#endif // !LOG_UTILS_H
