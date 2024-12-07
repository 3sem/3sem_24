#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <assert.h>
#include <limits.h>
#include <ftw.h>
#include <errno.h>
#include "tmp_dir_st.h"
#include "debugging.h"

int create_tmp_dir(const char *path);

void clear_tmp(const char *tmp_path, int flag);

int delete_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int open_file_in_tmp_dir(const char *filename, const char *tmp_path);

int handle_pid_change(tmp_st *self);

void tmp_dir_destr(tmp_st *self, const int delete_dir_bool);

int open_diff_file(tmp_st *self);


int tmp_dir_init(tmp_st **tmp, const char *path)
{
    RETURN_ON_TRUE(create_tmp_dir(path) == -1, -1);

    tmp_st *st_holder = (tmp_st *)calloc(1, sizeof(tmp_st));
    RETURN_ON_TRUE(!st_holder, TMP_ST_INIT_ERR, 
        printf(MEM_ALC_ERR_MSG););
    
    st_holder->tmp_dir = (char *)calloc(PATH_MAX, sizeof(char));
    RETURN_ON_TRUE(!st_holder->tmp_dir, TMP_ST_INIT_ERR, free(st_holder););

    memcpy(st_holder->tmp_dir, path, PATH_MAX * sizeof(char));

    st_holder->new_data_fd = open_file_in_tmp_dir(STANDARD_NEW_DATA_FILE_NAME, path);
    RETURN_ON_TRUE(st_holder->new_data_fd == -1, -1, free(st_holder->tmp_dir); free(st_holder););
    st_holder->old_data_fd = open_file_in_tmp_dir(STANDARD_OLD_DATA_FILE_NAME, path);
    RETURN_ON_TRUE(st_holder->old_data_fd == -1, -1, free(st_holder->tmp_dir); free(st_holder););

    st_holder->diff_counter = 0;
    
    st_holder->methods.pid_change_update  = handle_pid_change;
    st_holder->methods.dir_desctr         = tmp_dir_destr;
    st_holder->methods.open_new_diff      = open_diff_file;

    *tmp = st_holder;

    return 0;
}

void tmp_dir_destr(tmp_st *self, const int delete_dir_bool)
{
    if (delete_dir_bool)
        clear_tmp(self->tmp_dir, FTW_DEPTH | FTW_PHYS);

    close(self->new_data_fd);
    close(self->old_data_fd);
    close(self->curr_diff_fd);

    free(self->tmp_dir);

    free(self);
}

int handle_pid_change(tmp_st *self)
{
    close(self->new_data_fd);
    close(self->old_data_fd);
    close(self->curr_diff_fd);

    self->curr_diff_fd = 0;

    create_tmp_dir(self->tmp_dir);

    self->diff_counter = 0;

    self->new_data_fd = open_file_in_tmp_dir(STANDARD_NEW_DATA_FILE_NAME, self->tmp_dir);
    RETURN_ON_TRUE(self->new_data_fd == -1, -1, free(self->tmp_dir); free(self););
    self->old_data_fd = open_file_in_tmp_dir(STANDARD_OLD_DATA_FILE_NAME, self->tmp_dir);
    RETURN_ON_TRUE(self->old_data_fd == -1, -1, free(self->tmp_dir); free(self););

    return 0;
}

int open_diff_file(tmp_st *self)
{
    char path[PATH_MAX] = {0};
    snprintf(path, PATH_MAX * sizeof(char), "%s/%ld%s", self->tmp_dir, self->diff_counter, STANDARD_DIFF_OUTPUT_FORMAT);

    self->curr_diff_fd = open(path, O_CREAT | O_WRONLY, 0777);
    LOG("> diff fd is %d\n", diff_fd);
    RETURN_ON_TRUE(self->curr_diff_fd == -1, -1, perror("diff file opening error"););

    return 0;
}

int open_file_in_tmp_dir(const char *filename, const char *tmp_path)
{
    assert(filename);

    char path[PATH_MAX] = {};
    snprintf(path, PATH_MAX * sizeof(char), "%s/%s", tmp_path, filename);

    int fd = open(path, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
        perror("tmp file opening error");

    return fd;
}

int create_tmp_dir(const char *path)
{
    clear_tmp(path, FTW_DEPTH | FTW_PHYS);

    int ret_val = mkdir(path, S_IFDIR | 0777);
    if (ret_val == -1 && errno == EEXIST)
    {
        errno = 0;
        ret_val = 0;
    }
    
    return ret_val;
}

void clear_tmp(const char *tmp_path, int flag)
{
    RETURN_ON_TRUE(nftw(tmp_path, delete_file, 10, flag) == -1,);
}

int delete_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int ret_val = remove(path);
    RETURN_ON_TRUE(ret_val == -1, -1, perror("file or directory delete error"););

    (void)sb;
    (void)typeflag;
    (void)ftwbuf;

    return 0;
}