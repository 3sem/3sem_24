#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "parse.h"
#include "pipe.h"

int main (int argc, char* argv[])
{
    char const* sourcePath = argv[1];

    int sourceDescriptor = open (sourcePath, O_RDONLY);

    Text cmds = {};

    CreateText (sourceDescriptor, &cmds);

    Pipeline (&cmds, STDIN_FILENO);

    DestroyText (&cmds);
}
