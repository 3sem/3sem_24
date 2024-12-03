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

int functional_process(const pid_t pid_to_monitor, const int fd)
{
    RETURN_ON_TRUE(signal_handler_set(technical_sigint, SIGINT) == -1, -1);

    int ret_val                         = 0;
    config_st config                    = {};
    config.monitoring_pid               = pid_to_monitor;

    load_standard_config(&config);

    char path[PATH_MAX]                 = {0};
    snprintf(path, PATH_MAX * sizeof(char), "./%d.txt", pid_to_monitor);

    LOG("the result of combination is: %s\n", path);

    ret_val = create_tmp_dir();
    RETURN_ON_TRUE(ret_val == -1, -1, perror("couldn't create temp directory"););
    while (1)
    {
        RETURN_ON_TRUE(check_technical_signals(), 1, clear_tmp(););

        sleep(config.period);

        ret_val = update_config(&config, fd);
        if (ret_val)
            break;

        ret_val = file_diff(path);
        if (ret_val)
            break;
        
        //RETURN_ON_TRUE(write(config.diff_file_fd, ">\n", 2 * sizeof(char)) == -1, -1, perror("couldn't write data"););
    }
    clear_tmp();

    close(config.diff_file_fd);
    close(fd);
    
    return ret_val;
}