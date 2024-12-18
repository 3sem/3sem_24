#ifndef COMMON_FUNC
#define COMMON_FUNC

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 4092

#define RETURN_ERROR_ON_TRUE(statement, return_val, ...)                \
    do                                                                  \
    {                                                                   \
        if (statement)                                                  \
        {                                                               \
            __VA_ARGS__                                                 \
            return return_val;                                          \
        }                                                               \
    } while (0)
    
    
#define LOG(...) //fprintf(stderr, __VA_ARGS__)

#ifdef TIMER
#define PRINT_TIME(...) printf(__VA_ARGS__);
#else
#define PRINT_TIME(...) do {} while(0)
#endif

void start_timer();

long long int stop_timer();

off_t get_file_size(int fd);

#endif