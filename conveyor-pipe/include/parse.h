#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

typedef struct
{
    char** argv;
    size_t argc;
} Command;

typedef struct
{
    char*  buffer;
    size_t bufferSize;

    Command* cmds;
    size_t          cmdsCount;
    size_t          iCmd;
} Text;

int CreateText  (int fd, Text* inputText);

int DestroyText (Text* inputText);

#endif // PARSE_H
