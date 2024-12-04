#ifndef CFG_FILE_FUNCTIONS
#define CFG_FILE_FUNCTIONS

#include <sys/types.h>
#include "parse_standard_config.h"

#define CONFIG_FILE_PATH   "./processmon_config.cfg"

enum FIFO_DATA_TYPES
{
    PID             = 1,
    PERIOD          = 2,
    DIFF_FILE_FD    = 3,
    SAVE_CFG
};

int create_ipc_fifo();

void destruct_ipc_fifo();

int update_config(config_st *config, const int fd_r);

int change_config(const int fd_w, const int option, const void *data, size_t size);

#endif