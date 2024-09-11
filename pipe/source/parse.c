#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>

#include "parse.h"

static size_t   getSize            (int fd);

static char*    parseBuffer        (int fd, Text* inputText);

static size_t   countTokens        (char* buffer, char sepSymbol);

static Command* getCommands        (char* buffer, size_t cmdsCount);

static char**   getCommandArgVal   (char* buffer, size_t argc);

static size_t   getCommandArgCount (char* buffer);


int CreateText (int fd, Text* inputText)
{
    assert (inputText);

    inputText->bufferSize = getSize (fd);

    inputText->buffer     = parseBuffer (fd, inputText);

    inputText->cmdsCount  = countTokens (inputText->buffer, '|');

    inputText->cmds       = getCommands (inputText->buffer, inputText->cmdsCount);

    for (size_t iCmd = 0; iCmd < inputText->cmdsCount; iCmd++)
    {
        char** argv = inputText->cmds[iCmd].argv;
        size_t argc = inputText->cmds[iCmd].argc;
    }

    inputText->iCmd = 0;

    return 0;
}

int DestroyText (Text* inputText)
{
    assert (inputText);

    for (int iCmd = 0; iCmd < inputText->cmdsCount; iCmd++)
    {
        free(inputText->cmds[iCmd].argv);
    }

    inputText->bufferSize = 0xDEADDEAD;
    inputText->cmdsCount  = 0xDEADDEAD;

    free (inputText->buffer);
    free (inputText->cmds);

    return 0;
}

static size_t getSize (int fd)
{
    struct stat info = {};

    if (fstat(fd, &info) == -1)
    {
        return 0;
    }

    return (size_t) info.st_size;
}

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

static char* parseBuffer (int fd, Text* inputText)
{
    assert (inputText);

    RETURN_ERROR_OR_CONTINUE(lseek(fd, 0, SEEK_SET) == -1,
                             "Error: requested file position beyond the end of the file", NULL
                            );

    RETURN_ERROR_OR_CONTINUE(inputText->bufferSize == 0,
                             "Error: file size is zero.", NULL
                            );

    char* buffer = (char*) calloc (inputText->bufferSize + 1, sizeof(char));

    RETURN_ERROR_OR_CONTINUE(buffer == NULL,
                             "Error: unable to allocate buffer memory.", NULL
                            );

    ssize_t numBytesRead = read (fd, buffer, inputText->bufferSize);

    RETURN_ERROR_OR_CONTINUE(numBytesRead == -1,
                             "Error: unable to read from file.", NULL,
                             free (buffer);
                            );

    RETURN_ERROR_OR_CONTINUE(numBytesRead != inputText->bufferSize,
                             "Error: number of bytes read isn't correct", NULL,
                             free (buffer);
                            );

    char* hoursOfDebugSymbol = strchr(buffer, '\n');

    if (hoursOfDebugSymbol != NULL)
    {
        *hoursOfDebugSymbol = '\0';
    }

    return buffer;
}

static size_t countTokens (char* buffer, char sepSymbol)
{
    assert (buffer);

    char* begin = buffer;
    char* end   = buffer;

    size_t count = 0;

    while ((end = strchr (begin, sepSymbol)) != NULL)
    {
        begin = end + 1;

        count++;
    }

    if (begin != buffer) // Ignore the one command (no pipeline)
    {
        count++;
    }

    RETURN_ERROR_OR_CONTINUE(count == 0, "Error: no commands", 0);

    return count;
}

static Command* getCommands (char* buffer, size_t cmdsCount)
{
    assert (buffer);

    Command* cmds = (Command*) calloc (cmdsCount, sizeof(Command));

    RETURN_ERROR_OR_CONTINUE(cmds == NULL, "Error, unable to allocate memory", NULL);

    char* cmdBegin = buffer;
    char* cmdEnd   = buffer;

    size_t iCmd = 0;

    for (; iCmd < cmdsCount - 1; iCmd++)
    {
        cmdEnd = strchr (cmdBegin, '|');
        *cmdEnd = '\0';

        while (isspace (*cmdBegin))
        {
            cmdBegin++;
        }

        cmds[iCmd].argc = getCommandArgCount (cmdBegin);
        cmds[iCmd].argv = getCommandArgVal   (cmdBegin, cmds[iCmd].argc);

        cmdBegin = cmdEnd + 1;
    }

    while (isspace(*cmdBegin))
    {
        cmdBegin++;
    }

    cmds[iCmd].argc = getCommandArgCount (cmdBegin);
    cmds[iCmd].argv = getCommandArgVal   (cmdBegin, cmds[iCmd].argc);

    return cmds;
}

static size_t getCommandArgCount (char* buffer)
{
    assert (buffer);

    char* begin = buffer;
    char* end   = buffer;

    size_t argc = 1;

    while ((end = strchr (begin, ' ')) != NULL)
    {
        begin = end + 1;

        while (isspace (*begin))
        {
            begin++;
        }

        if (*begin == '\0')
        {
            break;
        }

        argc++;
    }

    return argc;
}

static char** getCommandArgVal (char* buffer, size_t argc)
{
    char** argv = (char**) calloc (argc + 1, sizeof(char*)); // +1 last argv is null

    RETURN_ERROR_OR_CONTINUE(argv == NULL, "Error: unable to allocate memory", NULL);

    https://stackoverflow.com/questions/18547114/why-do-we-need-argc-while-there-is-always-a-null-at-the-end-of-argv

    argv[argc] = NULL;

    char* begin = buffer;
    char* end = buffer;

    size_t iArg = 0;

    for (; iArg < argc - 1; iArg++) // last command does or doesn't have args after
    {
        end = strchr (begin, ' ');

        if (end != NULL)
        {
            *end = '\0';
        }

        argv[iArg] = begin;

        begin = end + 1;

        while (isspace (*begin))
        {
            begin++;
        }

        if (*begin == '\0')
        {
            break;
        }
    }

    argv[iArg] = begin;

    end = strchr (begin, ' ');

    if (end != NULL)
    {
        *end = '\0';
    }

    return argv;
}

#undef RETURN_ERROR_OR_CONTINUE




