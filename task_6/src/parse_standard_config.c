#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "parse_standard_config.h"
#include "debugging.h"

int create_config_file(const char *path);

int load_standard_config(config_st * cfg)
{
    char path[PATH_MAX] = {};
    snprintf(path, PATH_MAX * sizeof(char), STANDARD_CONFIG_NAME, BIN_PATH);

    int cfg_fd = open(path, O_RDONLY);
    if (cfg_fd == -1 && errno == ENOENT)
    {
        errno = 0;

        cfg_fd = create_config_file(path);
        RETURN_ON_TRUE(cfg_fd == -1, -1, close(cfg_fd););
    }
    RETURN_ON_TRUE(cfg_fd == -1, -1, perror("config file open error\n"););

    //parse_config(cfg);
    
    cfg->period         = STANDARD_PERIOD;
    cfg->diff_file_fd   = 1;//STANDARD_FILE_OUTPUT;

    close(cfg_fd);
    return 0;
}

int create_config_file(const char *path)
{
    printf("Processmon: creating standard config file\n");

    int cfg_fd = open(path, O_CREAT | O_WRONLY, 0777);
    RETURN_ON_TRUE(cfg_fd == -1, -1, perror("config file creation error"););

    char buff[CONFIG_FILE_MAX_SIZE] = {0};
    snprintf(buff, CONFIG_FILE_MAX_SIZE * sizeof(char), CONFIG_FILE_STRUCTURE, STANDARD_PERIOD, STANDARD_FILE_OUTPUT, STANDARD_TMP_PATH);

    ssize_t bytes_written = write(cfg_fd, buff, strlen(buff) * sizeof(char));
    RETURN_ON_TRUE(bytes_written == -1, -1, perror("couldn't write to config file"););

    int err_num = fcntl(cfg_fd, F_SETFD, O_RDONLY);
    RETURN_ON_TRUE(err_num == -1, -1, perror("couldn't change config file flags to read"););

    return cfg_fd;
}