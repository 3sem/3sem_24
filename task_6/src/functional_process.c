#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include "debugging.h"
#include "sig_handlers.h"
#include "parameters_changing.h"
#include "functional_part.h"
#include "parse_standard_config.h"
#include "find_file_diff.h"
#include "tmp_dir_st.h"

int functional_process(const int pid_to_monitor, const int ipc_fd)
{
    RETURN_ON_TRUE(signal_handler_set(technical_sigint, SIGINT) == -1, -1);

    int ret_val                         = 0;
    config_st config                    = {};
    RETURN_ON_TRUE(init_cfg_struct(&config) == MEM_ALC_ERR, MEM_ALC_ERR,
        printf(MEM_ALC_ERR_MSG););

    config.monitoring_pid               = pid_to_monitor;

    RETURN_ON_TRUE(load_standard_config(&config) == -1, -1, destr_cfg_struct(&config););

    tmp_st *temp_dir = NULL;
    ret_val = tmp_dir_init(&temp_dir, config.tmp_folder_path);
    RETURN_ON_TRUE(ret_val, ret_val, 
        perror("couldn't create temp directory");
        destr_cfg_struct(&config););

    while (1)
    {
        if (check_technical_signals())
            break;

        sleep(config.period);

        ret_val = update_config(&config, temp_dir, ipc_fd);
        if (ret_val && ret_val != TMP_CNG_ERR)
            break;

        ret_val = file_diff(config.monitoring_pid, temp_dir);
        if (ret_val && ret_val != TMP_CNG_WAIT)
            break;
    }

    close(ipc_fd);
    temp_dir->methods.dir_desctr(temp_dir, config.tmp_delete_bool);
    destr_cfg_struct(&config);
    
    return ret_val;
}