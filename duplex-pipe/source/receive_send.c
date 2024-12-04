#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "duplex_pipe.h"
#include "receive_send.h"

//

const char* parentSendFilename   = "examples/4gbParentSend.bd";
const char* childReceiveFilename = "examples/4gbChildReceive.bd";

const char* childSendFilename     = "examples/4gbChildSend.bd";
const char* parentReceiveFilename = "examples/4gbParentReceive.bd";

//

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

#define LOG(...) fprintf(stderr, __VA_ARGS__)

//

typedef struct _file_t
{
    int    fd;
    size_t size;

} file_t;

static file_t openFile    (const char* filename);

static size_t getFileSize (const char* filename);

static size_t closeFile   (file_t* file);

//

size_t childReceive (Pipe* dpipe)
{
    assert (dpipe);

    file_t file = openFile (childReceiveFilename);

    ON_DEBUG(LOG("Child started receiving data...\n"));

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdForw[1]) == -1, "Error: unable to close read end for child side send", -1);

    size_t readSize = 0;
    ON_DEBUG(size_t totalSize = 0);

    while ((readSize = read (dpipe->fdForw[0], dpipe->data, dpipe->dataCapacity)))
    {
        RETURN_ERROR_OR_CONTINUE(readSize == -1, "Error: unable to read from file", -1);

        dpipe->dataSize = readSize;

        ON_DEBUG(totalSize += readSize);

        RETURN_ERROR_OR_CONTINUE(write (file.fd, dpipe->data, dpipe->dataSize) == -1 ,
                                 "Error: unable to write to child file", -1);

        if (dpipe->dataSize < dpipe->dataCapacity)
        {
            break;
        }
    }

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdForw[0]) == -1, "Error: unable to close write end for child side send", -1);

    ON_DEBUG(LOG("Child finished receiving data... %zu bytes received...\n", totalSize));

    closeFile (&file);

    return 0;
}

size_t parentReceive (Pipe* dpipe)
{
    assert (dpipe);

    file_t file = openFile (parentReceiveFilename);

    ON_DEBUG(LOG("Parent started receiving data...\n"));

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdBack[1]) == -1, "Error: unable to close read end for child side send", -1);

    size_t readSize = 0;
    ON_DEBUG(size_t totalSize = 0);

    while ((readSize = read (dpipe->fdBack[0], dpipe->data, dpipe->dataCapacity)))
    {
        RETURN_ERROR_OR_CONTINUE(readSize == -1, "Error: unable to read from file", -1);

        dpipe->dataSize = readSize;

        ON_DEBUG(totalSize += readSize);

        RETURN_ERROR_OR_CONTINUE(write (file.fd, dpipe->data, dpipe->dataSize) == -1,
                                 "Error: unable to write to parent file", -1);

        if (dpipe->dataSize < dpipe->dataCapacity)
        {
            break;
        }
    }

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdBack[0]) == -1, "Error: unable to close write end for child side send", -1);

    ON_DEBUG(LOG("Parent finished receiving data... %zu bytes received...\n", totalSize));

    closeFile (&file);

    return 0;
}

size_t childSend (Pipe* dpipe)
{
    assert (dpipe);

    file_t file = openFile (childSendFilename);

    ON_DEBUG(LOG("Child started sending data...\n"));

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdBack[0]) == -1, "Error: unable to close read end for child side receive", -1);

    size_t readSize = 0;
    ON_DEBUG(size_t totalSize = 0);

    while ((readSize = read(file.fd, dpipe->data, dpipe->dataCapacity)))
    {
        RETURN_ERROR_OR_CONTINUE(readSize == -1, "Error: unable to read from file", -1);

        dpipe->dataSize = readSize;

        ON_DEBUG(totalSize += readSize);

        RETURN_ERROR_OR_CONTINUE(write (dpipe->fdBack[1], dpipe->data, dpipe->dataSize) == -1,
                                 "Error: unable to write to write end of fdBack", -1);

        if (dpipe->dataSize < dpipe->dataCapacity)
        {
            break;
        }
    }

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdBack[1]) == -1, "Error: unable to close write end for child side receive", -1);

    ON_DEBUG(LOG("Child finished sending data... %zu bytes sent...\n", totalSize));

    closeFile (&file);

    return 0;
}

size_t parentSend (Pipe* dpipe)
{
    assert (dpipe);

    file_t file = openFile (parentSendFilename);

    ON_DEBUG(LOG("Parent started sending data...\n"));

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdForw[0]) == -1, "Error: unable to close read end for parent side send", -1);

    size_t readSize = 0;
    ON_DEBUG(size_t totalSize = 0);

    while ((readSize = read (file.fd, dpipe->data, dpipe->dataCapacity)))
    {
        RETURN_ERROR_OR_CONTINUE(readSize == -1, "Error: unable to read from file", -1);

        dpipe->dataSize = readSize;

        ON_DEBUG(totalSize += readSize);

        RETURN_ERROR_OR_CONTINUE(write (dpipe->fdForw[1], dpipe->data, dpipe->dataSize) == -1,
                                 "Error: unable to write to fdForw", -1);

        if (dpipe->dataSize < dpipe->dataCapacity)
        {
            break;
        }
    }

    RETURN_ERROR_OR_CONTINUE(close (dpipe->fdForw[1]) == -1, "Error: unable to close write end for parent side send", -1);

    ON_DEBUG(LOG("Parent finished sending data... %zu bytes sent...\n", totalSize));

    closeFile (&file);

    return 0;
}

//

static file_t openFile (const char* filename)
{
    assert (filename);

    file_t file = {.fd = -1, .size = 0};

    file.size = getFileSize (filename);
    file.fd   = open (filename, O_RDWR | O_CREAT);

    RETURN_ERROR_OR_CONTINUE(file.fd == -1, "Error: unable to open file", file);

    return file;
}

static size_t getFileSize (const char* filename)
{
    assert (filename);

    struct stat stats = {};
    stat (filename, &stats);

    return (size_t) stats.st_size;
}

static size_t closeFile (file_t* file)
{
    assert (file);

    file->size = 0;
    file->fd   = -1;

    close (file->fd);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE
#undef ON_DEBUG

