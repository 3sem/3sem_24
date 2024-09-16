#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifdef LOG
    #define PRINT_LOG(...) printf("[LOG] " __VA_ARGS__)
#else
    #define PRINT_LOG(...)
#endif


#ifdef DEBUG
    #define PRINT_DEBUG(...) { fprintf(stderr, "[DEBUG] " __VA_ARGS__); }
    #define PRINT_ERROR(...)                                                                     \
    {                                                                                            \
        fprintf(stderr, "[ERROR] " __VA_ARGS__);                                                 \
        fprintf(stderr, "FILE: %s; FUNC: %s; LINE: %d; \n", __FILE__, __func__, __LINE__);       \
    }
#else
    #define PRINT_DEBUG(...) {}
    #define PRINT_ERROR(...) {}
#endif

#endif // LOGGER_H
