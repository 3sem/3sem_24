#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "duplex_pipe.h"

#define RETURN_ERROR_OR_CONTINUE(expression, errorMessage, retValue, ...) \
{                                                                         \
    if (expression)                                                       \
    {                                                                     \
        perror (errorMessage);                                            \
                                                                          \
        __VA_ARGS__                                                       \
                                                                          \
        return retValue;                                                  \
    }                                                                     \
}                                                                         \

#ifdef DEBUG
    #define ON_DEBUG(...) \
        __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

const static size_t BUFFER_SIZE = 4096;

Pipe* CreateDuplexPipe (OpTable ops)
{
    Pipe* dpipe = (Pipe*) calloc (1, sizeof(Pipe));

    RETURN_ERROR_OR_CONTINUE(dpipe == NULL, "Error: unable to allocate duplex pipe", NULL);

    dpipe->data = (char*) calloc (BUFFER_SIZE, sizeof(char));

    RETURN_ERROR_OR_CONTINUE(dpipe->data == NULL, "Error: unable to allocate buffer for duplex pipe", NULL);


    dpipe->dataSize     = 0;
    dpipe->dataCapacity = BUFFER_SIZE;

    dpipe->ops = ops;

    RETURN_ERROR_OR_CONTINUE(pipe(dpipe->fdForw) == -1, "Error: unable to pipe fdForw", NULL);
    RETURN_ERROR_OR_CONTINUE(pipe(dpipe->fdBack) == -1, "Error: unable to pipe fdBack", NULL);

    return dpipe;
}

int DestroyDuplexPipe (Pipe* dpipe)
{
    assert (dpipe);

    dpipe->dataSize     = 0;
    dpipe->dataCapacity = 0;
    free (dpipe->data);

    dpipe->fdBack[0] = -1;
    dpipe->fdBack[1] = -1;

    dpipe->fdForw[0] = -1;
    dpipe->fdForw[1] = -1;

    dpipe->ops.receive = NULL;
    dpipe->ops.send    = NULL;

    free (dpipe);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE
#undef ON_DEBUG
