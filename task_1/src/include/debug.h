#ifndef DEBUG_ERRS
#define DEBUG_ERRS

#include "log.h"

enum CONS_EMUL_ERRS
{
    P_CREATE_ERR        = 1011,
    SCANF_ERR           = 1111,
    CMD_ARR_MEM_ALC_ERR = 4009,
    CMD_ARR_MEM_RLC_ERR = 4010,
    PARSE_ERR           = 4011,
    INPUT_READ_ERR      = 4012,
};

#endif