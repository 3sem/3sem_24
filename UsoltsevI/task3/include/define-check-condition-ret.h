#pragma once
//-----------------------------------------------------------------
//
// Check-condition-ret define header 
//
//-----------------------------------------------------------------

#define CHECK_CONDITION_RET(a, to_free1, to_free2, to_return)   \
    if (a) {                                    \
        fprintf(stderr                          \
                , "%s in func %s in file %s\n"  \
                , #a, __func__, __FILE__);      \
        free(to_free1);                         \
        free(to_free2);                         \
        return to_return;                       \
    }

#define CHECK_CONDITION_PRINT(a, message)       \
    if (a) {                                    \
        fprintf(stderr                          \
                , "%s in funct %s in file %s\n" \
                , #a, __func__, __FILE__);      \
    }
