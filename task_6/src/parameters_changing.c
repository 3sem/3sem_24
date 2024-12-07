#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <errno.h>
#include <poll.h>
#include "debugging.h"
#include "tmp_dir_st.h"
#include "change_tmp_dir.h"
#include "parameters_changing.h"

int create_ipc_fifo()
{
    int err = mkfifo(CONFIG_FILE_PATH, 0666 | O_CREAT | O_EXCL);
    RETURN_ON_TRUE(err == -1, -1, perror("fifo creation error"););

    int fd  = open(CONFIG_FILE_PATH, O_RDWR);
    RETURN_ON_TRUE(fd == -1, -1, perror("fifo file open error"););

    return fd;
}

void destruct_ipc_fifo()
{
    unlink(CONFIG_FILE_PATH);
}

int update_config(config_st *config, tmp_st *dir_st, const int fd_r)
{
    assert(config);

    struct pollfd fds = {};
    fds.fd       = fd_r;
    fds.events   = POLLIN;

    if (!poll(&fds, 1, 0))
        return 0;

    int config_parameter_type = 0;
    ssize_t error   = 0;
    int ret_val     = 0;

    error = read(fd_r, &config_parameter_type, sizeof(int));
    RETURN_ON_TRUE(error == -1, 0);

    switch (config_parameter_type)
    {
    case PID:
        error = read(fd_r, &config->monitoring_pid, sizeof(pid_t));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););
        ret_val = dir_st->methods.pid_change_update(dir_st);
        RETURN_ON_TRUE(ret_val, ret_val);
        break;
    
    case PERIOD:
        error = read(fd_r, &config->period, sizeof(unsigned int));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););
        RETURN_ON_TRUE(config->period == 0, PERIOD_ZERO, printf("processmon: period of monitoring can't be zero\n"););
        break;

    case DIFF_FILE_FD:
        char path[PATH_MAX] = {0};
        error = read(fd_r, path, PATH_MAX * sizeof(char));
        RETURN_ON_TRUE(error == -1, -1, perror("cfg file reading error"););

        RETURN_ON_TRUE(move_tmp_dir(path, config->tmp_folder_path) == -1, TMP_CNG_ERR,
            perror("Processmon: couldn't move tmp directory for some reason"););

        memcpy(config->tmp_folder_path, path, PATH_MAX * sizeof(char));

        break;

    case SAVE_CFG:
        RETURN_ON_TRUE(save_current_config(config) == -1, -1);
        break;

    default:
        LOG("[error]> error occured while recieving data\n");
        return DATA_RCV_ERR;
    }

    return 0;
}

int change_config(const int fd_w, const int option, const void *data, size_t size)
{
    assert(data || size == 0);

    ssize_t error   = write(fd_w, &option, sizeof(int));
    RETURN_ON_TRUE(error == -1, -1, perror("cfg file wrtiting error\n"););

    error           = write(fd_w, data, size);
    RETURN_ON_TRUE(error == -1, -1, perror("cfg file wrtiting error\n"););

    return 0;
}