#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "parameters_changing.h"
#include "functional_part.h"
#include "deamon.h"
#include "debugging.h"

int save_pid();

int run_deamon(const int pid_to_monitor)
{
    pid_t chld_pid = fork();
    RETURN_ON_TRUE(chld_pid == -1, -1);
    RETURN_ON_TRUE(chld_pid, 0);

    setsid();
    RETURN_ON_TRUE(save_pid() == -1, -1);
    umask(0);

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int fd = create_ipc_fifo();
    RETURN_ON_TRUE(fd == -1, -1, destruct_ipc_fifo(););

    return functional_process(pid_to_monitor, fd);
}

int save_pid()
{
    int fd = open(DEAMON_PID_DIR, O_CREAT | O_WRONLY, 0777);
    RETURN_ON_TRUE(fd == -1, -1, perror("pid file open error"););
    RETURN_ON_TRUE(lseek(fd, SEEK_SET, 0) == -1, -1);
    RETURN_ON_TRUE(ftruncate(fd, 0) == -1, -1);

    pid_t pid = getpid();
    RETURN_ON_TRUE(write(fd, &pid, sizeof(pid_t)) == -1, -1, perror("pid write error"););

    close(fd);

    return 0;
}