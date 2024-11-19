#ifndef DEBUG_H
#define DEBUG_H

#define LOG(...) fprintf(stderr, __VA_ARGS__)

#define RETURN_ON_TRUE(statement, ret_val, ...)             \
    do                                                      \
    {                                                       \
        if (statement)                                      \
        {                                                   \
            __VA_ARGS__                                     \
            return ret_val;                                 \
        }                                                   \
    } while (0)
    

enum ERRORS
{
    ARGC_NOT_TWO    = 11,
    ARGC_TOO_MANY   = 13,
    NON_EXIST_OPT   = 12,


};

#endif