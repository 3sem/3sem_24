#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <memory.h>
#include "debugging.h"
#include "find_file_diff.h"
#include "change_tmp_dir.h"
#include "config_structure.h"
#include "file_copying.h"
#include "tmp_dir_st.h"

int write_diff(tmp_st *dir_st);

int check_file_emptyness(int fd);

int prepare_file_diff(const int monitoring_pid, const tmp_st *dir_st);


int file_diff(const int monitoring_pid, tmp_st *dir_st)
{
    assert(dir_st);

    if (check_tmp_changing_status())
        return TMP_CNG_WAIT;

    int err_num = prepare_file_diff(monitoring_pid, dir_st);
    RETURN_ON_TRUE(err_num == -1, -1);

    err_num = write_diff(dir_st);
    RETURN_ON_TRUE(err_num == -1, -1);

    return 0;
}

int write_diff(tmp_st *dir_st)
{
    assert(dir_st);

    if (dir_st->curr_diff_fd == 0)
        RETURN_ON_TRUE(dir_st->methods.open_new_diff(dir_st) == -1, -1);

    if (check_file_emptyness(dir_st->curr_diff_fd))
    {
        close(dir_st->curr_diff_fd);
        (dir_st->diff_counter)++;

        RETURN_ON_TRUE(dir_st->methods.open_new_diff(dir_st) == -1, -1);
    }
    
    pid_t pid = fork();
    RETURN_ON_TRUE(pid == -1, -1, perror("pid error"););

    if (pid)
        return 0;

    dup2(dir_st->curr_diff_fd, STDOUT_FILENO);

    char path_one[PATH_MAX / 2] = {0};
    char path_two[PATH_MAX / 2] = {0};

    snprintf(path_one, PATH_MAX / 2 * sizeof(char), "%s/%s", dir_st->tmp_dir, STANDARD_OLD_DATA_FILE_NAME);
    snprintf(path_two, PATH_MAX / 2 * sizeof(char), "%s/%s", dir_st->tmp_dir, STANDARD_NEW_DATA_FILE_NAME);

    execl("/bin/diff", "diff", path_one, path_two, (char *)NULL);

    return 0;
}


int check_file_emptyness(int fd)
{
    if (get_file_size(fd))
        return 1;
    
    return 0;
}

int prepare_file_diff(const int current_pid, const tmp_st *dir_st)
{
    assert(dir_st);

    int map_fd      = 0;

    char path[PATH_MAX] = {0};
    snprintf(path, PATH_MAX * sizeof(char), "./%d.txt", current_pid);
    
    map_fd = open(path, O_RDONLY);
    //Проверку на пропажу файла
    RETURN_ON_TRUE(map_fd == -1, -1, perror("couldn't open monitoring file"););

    RETURN_ON_TRUE(ftruncate(dir_st->old_data_fd, 0) == -1, -1, perror("ftruncate error"););
    RETURN_ON_TRUE(lseek(dir_st->old_data_fd, SEEK_SET, 0) == -1, -1, perror("lseek error\n"););
    RETURN_ON_TRUE(file_to_file(dir_st->old_data_fd, dir_st->new_data_fd) == -1, -1);

    RETURN_ON_TRUE(ftruncate(dir_st->new_data_fd, 0) == -1, -1, perror("ftruncate error"););
    RETURN_ON_TRUE(lseek(dir_st->new_data_fd, SEEK_SET, 0) == -1, -1, perror("lseek error\n"););
    RETURN_ON_TRUE(file_to_file(dir_st->new_data_fd, map_fd) == -1, -1);

    close(map_fd);

    return 0;
}