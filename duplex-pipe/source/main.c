#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "duplex_pipe.h"
#include "receive_send.h"

int main ()
{
    OpTable ops = {.send = parentSend, .receive = childReceive};

    Pipe* dpipe = CreateDuplexPipe(ops);

    pid_t pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Unable to fork");

        return -1;
    }
    else if (pid == 0)
    {
        dpipe->ops.receive(dpipe);

        dpipe->ops.send = childSend;

        dpipe->ops.send(dpipe);
    }
    else
    {
        dpipe->ops.send(dpipe);

        dpipe->ops.receive = parentReceive;

        dpipe->ops.receive(dpipe);
    }

    DestroyDuplexPipe(dpipe);

    return 0;
}
