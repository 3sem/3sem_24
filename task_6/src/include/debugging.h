#ifndef DEBUG_H
#define DEBUG_H

#define LOG(...) //fprintf(stderr, __VA_ARGS__)

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
    ERR_PID         = -1,
    ARGC_NOT_TWO    = 11,
    NON_EXIST_OPT   = 12,
    ARGC_TOO_MANY   = 13,
    FILE_OPEN_ERR   = 14,
    DATA_RCV_ERR    = 15,
    PERIOD_ZERO     = 16,


};

enum RET_VALUES
{
    INTERACT_KILL        = 101,
};

#endif