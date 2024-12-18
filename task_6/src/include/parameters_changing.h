#ifndef CFG_FILE_FUNCTIONS
#define CFG_FILE_FUNCTIONS

#include <sys/types.h>
#include "parse_standard_config.h"
#include "tmp_dir_st.h"

#define CONFIG_FILE_PATH   "/tmp/processmon_param"

enum FIFO_DATA_TYPES
{
    PID             = 1,
    PERIOD          = 2,
    DIFF_FILE_FD    = 3,
    SAVE_CFG        = 5,
    SHOW_CFG        = 6,
    CNG_SAVE_BOOL   = 7,
};

int create_ipc_fifo();

void destruct_ipc_fifo();

int update_config(config_st *config, tmp_st *dir_st,const int fd_r);

int change_config(const int fd_w, const int option, const void *data, size_t size);

#endif