#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "debugging.h"
#include "sig_handlers.h"
#include "parameters_changing.h"
#include "functional_part.h"
#include "parse_standard_config.h"
#include "find_file_diff.h"

//ability to change standart config

int functional_process(const pid_t pid_to_monitor, const int ipc_fd)
{
    RETURN_ON_TRUE(signal_handler_set(technical_sigint, SIGINT) == -1, -1);

    int ret_val                         = 0;
    config_st config                    = {};
    RETURN_ON_TRUE(init_cfg_struct(&config) == MEM_ALC_ERR, MEM_ALC_ERR,
        printf("Processmon: couldn't allocate memory for temp directory path in config\n"););

    config.monitoring_pid               = pid_to_monitor;

    RETURN_ON_TRUE(load_standard_config(&config) == -1, -1, destr_cfg_struct(&config););

    char path[PATH_MAX]                 = {0};
    snprintf(path, PATH_MAX * sizeof(char), "./%d.txt", pid_to_monitor);

    LOG("the result of combination is: %s\n", path);

    ret_val = create_tmp_dir(config.tmp_folder_path);
    RETURN_ON_TRUE(ret_val == -1, -1, 
        perror("couldn't create temp directory");
        destr_cfg_struct(&config););

    while (1)
    {
        if (check_technical_signals())
            break;

        sleep(config.period);

        ret_val = update_config(&config, ipc_fd);
        if (ret_val && ret_val != TMP_CNG_ERR)
            break;

        ret_val = file_diff(path, config.tmp_folder_path);
        if (ret_val && ret_val != TMP_CNG_WAIT)
            break;
        
        //RETURN_ON_TRUE(write(config.diff_file_fd, ">\n", 2 * sizeof(char)) == -1, -1, perror("couldn't write data"););
    }
    if (config.tmp_delete_bool) 
        clear_tmp(config.tmp_folder_path);

    close(ipc_fd);
    destr_cfg_struct(&config);
    
    return ret_val;
}