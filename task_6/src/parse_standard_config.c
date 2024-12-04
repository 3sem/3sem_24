#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "parse_standard_config.h"
#include "find_file_diff.h"
#include "debugging.h"

int create_config_file(const char *path);

int parse_config(config_st *cfg, const int cfg_fd);

int load_standard_config(config_st *cfg)
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

    int ret_val = parse_config(cfg, cfg_fd);
    RETURN_ON_TRUE(ret_val == -1, -1);
    if (ret_val)
    {
        close(cfg_fd);
        unlink(path);

        return load_standard_config(cfg);
    }
    
    close(cfg_fd);
    return 0;
}

int save_current_config(const config_st *cfg)
{
    char path[PATH_MAX] = {};
    snprintf(path, PATH_MAX * sizeof(char), STANDARD_CONFIG_NAME, BIN_PATH);

    int cfg_fd = open(path, O_WRONLY, 0777);
    RETURN_ON_TRUE(cfg_fd == -1, -1, perror("config file open error"););

    RETURN_ON_TRUE(ftruncate(cfg_fd, 0) == -1, -1);
    RETURN_ON_TRUE(lseek(cfg_fd, SEEK_SET, 0) == -1, -1);

    char buff[CONFIG_FILE_MAX_SIZE] = {0};
    snprintf(buff, CONFIG_FILE_MAX_SIZE * sizeof(char), CONFIG_FILE_STRUCTURE, cfg->period, cfg->output_file_path, cfg->tmp_folder_path, cfg->tmp_delete_bool);

    ssize_t bytes_written = write(cfg_fd, buff, strlen(buff) * sizeof(char));
    RETURN_ON_TRUE(bytes_written == -1, -1, perror("couldn't write to config file"););

    return 0;
}

int create_config_file(const char *path)
{
    printf("Processmon: creating standard config file\n");

    int cfg_fd = open(path, O_CREAT | O_RDWR, 0777);
    RETURN_ON_TRUE(cfg_fd == -1, -1, perror("config file creation error"););

    char buff[CONFIG_FILE_MAX_SIZE] = {0};
    snprintf(buff, CONFIG_FILE_MAX_SIZE * sizeof(char), CONFIG_FILE_STRUCTURE, STANDARD_PERIOD, STANDARD_FILE_OUTPUT, STANDARD_TMP_PATH, STANDARD_TMP_DEL_BOOL);

    ssize_t bytes_written = write(cfg_fd, buff, strlen(buff) * sizeof(char));
    RETURN_ON_TRUE(bytes_written == -1, -1, perror("couldn't write to config file"););

    RETURN_ON_TRUE(lseek(cfg_fd, SEEK_SET, 0) == -1, -1);

    return cfg_fd;
}

int parse_config(config_st *cfg, const int cfg_fd)
{
    assert(cfg);

    char buff[CONFIG_FILE_MAX_SIZE] = {0};
    int tmp_del_holder              = 0;

    ssize_t bytes_read = read(cfg_fd, buff, CONFIG_FILE_MAX_SIZE * sizeof(char));
    RETURN_ON_TRUE(bytes_read == -1, -1, perror("config file reading error"););

    int scanned = sscanf(buff, CONFIG_FILE_STRUCTURE, \
        &cfg->period, cfg->output_file_path, cfg->tmp_folder_path, &tmp_del_holder);
    if (scanned != 4)
    {
        printf("> scanned: %d\n", scanned);
        printf("Processmon: couldn't parse config properly, attempting to create new config\n");
        return CFG_PARSE_ERR;
    }
    
    cfg->tmp_delete_bool = (char)tmp_del_holder;
    
    return 0;
}