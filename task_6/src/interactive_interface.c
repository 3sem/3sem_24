#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include "interacrtive_interface.h"
#include "debugging.h"
#include "config_changing_funcs.h"
#include "memory_watcher.h"

//РЎРѕР·РґР°С‘Рј РґРѕС‡РµСЂРЅРёР№ РїСЂРѕС†РµСЃСЃ, РєРѕС‚РѕСЂС‹Р№ Р±СѓРґРµС‚ СЂР°Р±РѕС‚Р°С‚СЊ РІ С„РѕРЅРµ
//РЈР±РёР№СЃС‚РІРѕ РїСЂРѕС†РµСЃСЃР° Р±СѓРґРµС‚ РїСЂРѕРёСЃС…РѕРґРёС‚СЊ С‡РµСЂРµР· СЃРёРіРЅР°Р»
//Р’Р·Р°РёРјРѕРґРµР№СЃС‚РІРёРµ СЃ РїСЂРѕС†РµСЃСЃРѕРј Р±СѓРґРµС‚ РїСЂРѕРёСЃС…РѕРґРёС‚СЊ С‡РµСЂРµР· pipe

unsigned int read_number_from_input();

int interact_with_user(int fd);

int run_interactive(pid_t pid_to_monitor)
{
    pid_t pid        = 0;

    int fd = create_cfg_fifo();
    RETURN_ON_TRUE(fd == -1, -1, destruct_cfg_fifo(););

    pid = fork();
    RETURN_ON_TRUE(pid == -1, -1,
        perror("fork error\n"););

    if (pid)
    {
        int running_flag = 0;
        while (!running_flag)
            running_flag = interact_with_user(fd);

        int wstatus = 0;
        waitpid(pid, &wstatus, 0);

        close(fd);
        destruct_cfg_fifo();
        return WEXITSTATUS(wstatus);
    }
    else
    {
        //sigaction СЃСЋРґР°

        for (int i = 0; i < 1; i++)
        {
            sleep(10);

            config_st config = {15, 15, 555};

            int error = update_config(&config, fd);
            RETURN_ON_TRUE(error, error);

            close(config.diff_file_fd);

            printf("%d<>%u<>%d\n", config.monitoring_pid, config.period, config.diff_file_fd);
        }
        
        close(fd);
        return 0;
    }
    
    
    return 0;
}

int interact_with_user(int fd)
{
    unsigned int choosen_option = 0;
    int error = 0;

    printf("\033[47;30m\nprocessmon: your programm is running in interactive mode\n");
    printf("[1] - change proccess for monitoring\n");
    printf("[2] - change the frequency of how often monitoring will happen\n");
    printf("[3] - change the output file\n");
    printf("[4] - end the programm\n");

    while (1)
    {
        choosen_option = read_number_from_input();
        if (1 <= choosen_option && choosen_option <= 4)
            break;
        
        printf("Input error, please enter a number beetwen 1 and 4\n");
    }
    
    switch (choosen_option)
    {
    case 1:
    {
        LOG("> changing the pid\n");
        pid_t new_pid = (pid_t)read_number_from_input();
        error = change_config(fd, PID, &new_pid, sizeof(pid_t));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 2:
    {
        LOG("changing the timing\n");
        unsigned int new_period = read_number_from_input();
        error = change_config(fd, PERIOD, &new_period, sizeof(unsigned int));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 3:
    {
        LOG("changing a filepath\n");
        char path[PATH_MAX] = {};
        scanf("%s", path);
        int output_fd = open(path, 0666 | O_CREAT);
        RETURN_ON_TRUE(output_fd == -1, -1, perror("new output file creation error\n"););

        error = change_config(fd, DIFF_FILE_FD, &output_fd, sizeof(int));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 4:
        printf("\033[0m\n");
        return 1;

    default:
        LOG("[error]> NON-existent option chosen\n");
        return NON_EXIST_OPT;
    }

    return 0;
}

unsigned int read_number_from_input()
{
    unsigned int number  = 0;
    int scanned = 0;

    while (1)
    {   
        scanned = scanf("%u", &number);
        if (scanned)
            break;
        
        printf("Input error, please enter a number\n");
        while (getchar() != '\n');
    }

    return number;
}