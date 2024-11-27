#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "debugging.h"
#include "sig_handlers.h"
#include "config_changing_funcs.h"
#include "standart_config.h"
#include "functional_part.h"
#include "inotify_func.h"

//ability to change standart config

int functional_process(const pid_t pid_to_monitor, const int fd)
{
    RETURN_ON_TRUE(signal_handler_set(technical_sigint, SIGINT) == -1, -1);

    int ret_val                         = 0;
    config_st config                    = {pid_to_monitor, STANDART_PERIOD, STANDART_FILE_OUTPUT};

    char path[PATH_MAX]                 = {0};
    snprintf(path, PATH_MAX * sizeof(char), "/%d.txt", pid_to_monitor);

    LOG("the result of combination is: %s\n", path);

    ret_val = init_inotify_struct(path);
    RETURN_ON_TRUE(ret_val == -1, -1);

    while (1)
    {
        RETURN_ON_TRUE(check_technical_signals(), 1);

        sleep(config.period);

        ret_val = update_config(&config, fd);
        if (ret_val)
            break;

        unsigned int inotif_mask = notify_about_change_in_file();
        if (inotif_mask & IN_MODIFY)
        {
            printf("file has changed\n");
        }
        else if (inotif_mask & IN_DELETE_SELF)
        {
            printf("file was closed\n");
        }

        RETURN_ON_TRUE(write(config.diff_file_fd, ">\n", 2 * sizeof(char)) == -1, -1, perror("couldn't write data"););

    }

    close(config.diff_file_fd);
    close(fd);
    
    return ret_val;
}