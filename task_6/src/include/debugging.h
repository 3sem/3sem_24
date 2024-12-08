#ifndef DEBUG_H
#define DEBUG_H

#include "printable_text.h"

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
    CFG_EMPTY       = 17,
    MEM_ALC_ERR     = 18,
    CFG_PARSE_ERR   = 19,
    TMP_CNG_ERR     = 20,
    TMP_CNG_WAIT    = 21,
    TMP_ST_INIT_ERR = 22,
    DIR_OPEN_ERR    = 23,
    FILE_DELETED    = 24

};

enum RET_VALUES
{
    INTERACT_KILL        = 101,
};

#endif