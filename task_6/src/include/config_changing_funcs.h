#ifndef CFG_FILE_FUNCTIONS
#define CFG_FILE_FUNCTIONS

#include <sys/types.h>

#define CONFIG_FILE_PATH   "./processmon_config.cfg"

typedef struct 
{
    pid_t           monitoring_pid;
    unsigned int    period;
    int             diff_file_fd;
} config_st;

enum FIFO_DATA_TYPES
{
    PID             = 1,
    PERIOD          = 2,
    DIFF_FILE_FD    = 3,
};

int create_cfg_fifo();

void destruct_cfg_fifo();

int update_config(config_st *config, const int fd_r);

int change_config(const int fd_w, const int option, const void *data, size_t size);

#endif