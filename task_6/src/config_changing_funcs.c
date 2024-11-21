#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include "debugging.h"
#include "config_changing_funcs.h"

int create_cfg_fifo()
{
    int err = mkfifo(CONFIG_FILE_PATH, 0666 | O_CREAT | O_EXCL);
    RETURN_ON_TRUE(err == -1, -1, perror("fifo creation error"););

    int fd  = open(CONFIG_FILE_PATH, O_RDWR);
    RETURN_ON_TRUE(fd == -1, -1, perror("fifo file open error"););

    return fd;
}

void destruct_cfg_fifo()
{
    unlink(CONFIG_FILE_PATH);
}

int update_config(config_st *config, const int fd_r)
{
    assert(config);

    struct pollfd fds = {};
    fds.fd       = fd_r;
    fds.events   = POLLIN;

    if (!poll(&fds, 1, 0))
        return 0;

    int config_parameter_type = 0;
    ssize_t error = 0;

    error = read(fd_r, &config_parameter_type, sizeof(int));
    RETURN_ON_TRUE(error == -1, 0);

    switch (config_parameter_type)
    {
    case PID:
        error = read(fd_r, &config->monitoring_pid, sizeof(pid_t));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););
        break;
    
    case PERIOD:
        error = read(fd_r, &config->period, sizeof(unsigned int));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););
        RETURN_ON_TRUE(config->period == 0, PERIOD_ZERO, printf("processmon: period of monitoring can't be zero\n"););
        break;

    case DIFF_FILE_FD:
        close(config->diff_file_fd);

        error = read(fd_r, &config->diff_file_fd, sizeof(int));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););
        break;

    default:
        LOG("[error]> error occured while recieving data\n");
        return DATA_RCV_ERR;
    }

    return 0;
}

int change_config(const int fd_w, const int option, const void *data, size_t size)
{
    assert(data);

    ssize_t error   = write(fd_w, &option, sizeof(int));
    RETURN_ON_TRUE(error == -1, -1, perror("cfg file wrtiting error\n"););

    error           = write(fd_w, data, size);
    RETURN_ON_TRUE(error == -1, -1, perror("cfg file wrtiting error\n"););

    return 0;
}