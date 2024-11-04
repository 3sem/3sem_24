#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "pipe.h"
#include "parse.h"

static int runCmd (Text* cmds);

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

int Pipeline (Text* cmds, int inputFd)
{
    assert (cmds);

    ON_DEBUG(LOG("iCmd: %d\n", cmds->iCmd));

    if (cmds->iCmd != cmds->cmdsCount - 1)
    {
        int fds[2] = {};

        RETURN_ERROR_OR_CONTINUE(pipe (fds) == -1,
                                 "Error: unable to create pipe", -1);

        int readDescriptor  = fds[0];
        int writeDescriptor = fds[1];

        pid_t writePid = 0;
        RETURN_ERROR_OR_CONTINUE((writePid = fork ()) == -1,
                                 "Error: unable to fork", -1);

        ON_DEBUG(LOG("%d: %d\n", getpid(), writePid));

        if (writePid == 0)
        {
            ON_DEBUG(LOG("hello, %d: %d\n", getpid(), writePid));

            RETURN_ERROR_OR_CONTINUE(dup2 (writeDescriptor, STDOUT_FILENO) == -1,
                                           "Error: unable to dup", -1);

            if (inputFd != STDIN_FILENO)
            {
                RETURN_ERROR_OR_CONTINUE(dup2 (inputFd, STDIN_FILENO) == -1,
                                         "Error: unable to dup", -1);
            }

            RETURN_ERROR_OR_CONTINUE(close (fds[0]) == -1,
                                     "Error: unable to close read end", -1);
            RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                     "Error: unable to close write end", -1);

            ON_DEBUG(LOG("bye bye, %d: %d\n", getpid(), writePid));

            runCmd (cmds);
        }
        else
        {
            pid_t readPid = 0;
            RETURN_ERROR_OR_CONTINUE((readPid = fork ()) == -1,
                                     "Error: unable to fork", -1);

            if (readPid == 0)
            {
                RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                         "Error: unable to close write end", -1);

                // Proceed to next command

                cmds->iCmd++;
                Pipeline (cmds, readDescriptor);
            }
            else
            {
                RETURN_ERROR_OR_CONTINUE(close (fds[0]) == -1,
                                         "Error: unable to close read end", -1);
                RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                         "Error: unable to close write end", -1);

                int status = 0;

                waitpid (writePid, &status, 0);
                ON_DEBUG(LOG("%d status: %d\n", getpid(), WEXITSTATUS(status)));

                waitpid (readPid, &status, 0);
                ON_DEBUG(LOG("%d status: %d\n", getpid(), WEXITSTATUS(status)));
            }
        }
    }
    else
    {
        pid_t pid = 0;
        RETURN_ERROR_OR_CONTINUE((pid = fork ()) == -1,
                                 "Error: unable to fork", -1);

        if (pid == 0)
        {
            ON_DEBUG(LOG("fd: %d, %d\n", inputFd, getpid()));

            RETURN_ERROR_OR_CONTINUE(dup2 (inputFd, STDIN_FILENO) == -1,
                                     "Error: unable to dup", -1);

            RETURN_ERROR_OR_CONTINUE(close (inputFd) == -1,
                                     "Error: unable to close fd", -1);

            runCmd (cmds);
        }
        else
        {
            int status = 0;

            waitpid(pid, &status, 0);
            ON_DEBUG(LOG("%d status: %d\n", getpid(), WEXITSTATUS(status)));
        }
    }

    return 0;
}

static int runCmd (Text* cmds)
{
    ON_DEBUG(LOG("executing command [%zu]\n", cmds->iCmd));

    assert (cmds);

    char** argv = cmds->cmds[cmds->iCmd].argv;
    size_t argc = cmds->cmds[cmds->iCmd].argc;

    for (size_t iArg = 0; iArg < argc; iArg++)
    {
        printf ("%s ", argv[iArg]);
    }

    fflush (stdout);

    RETURN_ERROR_OR_CONTINUE(execvp (argv[0], argv) == -1,
                                     "Error: unable to exec", -1);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE

