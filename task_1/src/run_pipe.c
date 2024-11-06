#include <run_pipe.h>

void exec_cmd(char ***cmd)
{
    pid_t pid = 0;
    int fds[2] = {0};
    int fd_in = STDIN_FILENO;

    int cmd_count = 0;

    while (cmd[cmd_count] != NULL)
    {
        LOG("executing pipes:");

        if (pipe(fds))
        {
            perror("pipe creating error");
            return;
        }

        LOG("pipe created");
        LOG("Descriptors:\n1) %d\n2) %d\n", fds[0], fds[1]);
        LOG("Input fds %d\n", fd_in);

        pid = fork();
        if (pid == -1)
        {
            perror("process creating error");
            return;
        }
        else if (pid == 0)
        {
            if (cmd_count > 0)
            {
                dup2(fd_in, STDIN_FILENO);
                LOG("stdin file dup executed");
            }
            if (cmd[cmd_count + 1] != NULL)
            {
                dup2(fds[1], STDOUT_FILENO);
                LOG("stdout dup executed");
            }
            close(fds[0]);
            execvp(cmd[cmd_count][0], cmd[cmd_count]);
        }
        else
        {
            wait(NULL);
            close(fds[1]);
            fd_in = fds[0];
            cmd_count++;
        }
    }

    return;
}