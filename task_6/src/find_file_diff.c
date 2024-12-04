#define _GNU_SOURCE

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <memory.h>
#include "debugging.h"
#include "find_file_diff.h"
#include "config_structure.h"

static int diff_fd = 0;


int delete_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int write_diff(const char *tmp_path);

int open_diff_file(const int file_number, const char *tmp_path);

int check_file_emptyness(int fd);

int prepare_file_diff(const char *path, const char *tmp_path);

int open_file_in_tmp_dir(const char *filename, const char *tmp_path);

int file_to_file(const int to_fd, const int from_fd);


int file_diff(const char *path, const char *tmp_path)
{
    assert(path);

    int err_num = prepare_file_diff(path, tmp_path);
    RETURN_ON_TRUE(err_num == -1, -1);

    err_num = write_diff(tmp_path);
    RETURN_ON_TRUE(err_num == -1, -1);

    return 0;
}

// Переместить временную директорию....

int create_tmp_dir(const char *path)
{
    //Сделать проверку на существование директории
    int ret_val = mkdir(path, S_IFDIR | 0777);
    if (ret_val == -1 && errno == EEXIST)
    {
        errno = 0;
        ret_val = 0;
    }
    
    return ret_val;
}

void clear_tmp(const char *tmp_path)
{
    RETURN_ON_TRUE(nftw(tmp_path, delete_file, 10, FTW_DEPTH | FTW_PHYS) == -1,, printf("Processmon: error occured while deleting temporary files\n"););
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

//можно сделать структуру дифф файла с методами
int write_diff(const char *tmp_path)
{
    static int diff_counter = 0;

    if (diff_fd == 0)
    {
        diff_fd = open_diff_file(diff_counter, tmp_path);
        RETURN_ON_TRUE(diff_fd == -1, -1);
    }

    if (check_file_emptyness(diff_fd))
    {
        close(diff_fd);
        diff_counter++;

        diff_fd = open_diff_file(diff_counter, tmp_path);
        RETURN_ON_TRUE(diff_fd == -1, -1);
    }
    
    pid_t pid = fork();
    RETURN_ON_TRUE(pid == -1, -1, perror("pid error"););

    if (pid)
        return 0;

    dup2(diff_fd, STDOUT_FILENO);

    char path_one[PATH_MAX / 2] = {0};
    char path_two[PATH_MAX / 2] = {0};

    snprintf(path_one, PATH_MAX / 2 * sizeof(char), "%s/%s", tmp_path, STANDARD_OLD_DATA_FILE_NAME);
    snprintf(path_two, PATH_MAX / 2 * sizeof(char), "%s/%s", tmp_path, STANDARD_NEW_DATA_FILE_NAME);

    execl("/bin/diff", "diff", path_one, path_two, (char *)NULL);

    return 0;
}

int open_diff_file(const int file_number, const char *tmp_path)
{
    char path[PATH_MAX] = {0};
    snprintf(path, PATH_MAX * sizeof(char), "%s/%d%s", tmp_path, file_number, STANDARD_DIFF_OUTPUT_FORMAT);

    diff_fd = open(path, O_CREAT | O_WRONLY, 0777);
    LOG("> diff fd is %d\n", diff_fd);
    RETURN_ON_TRUE(diff_fd == -1, -1, perror("diff file opening error"););

    return diff_fd;
}


int check_file_emptyness(int fd)
{
    if (get_file_size(fd))
        return 1;
    
    return 0;
}

int prepare_file_diff(const char *path, const char *tmp_path)
{
    static int func_init = 1;
    static int old_data_fd = 0;
    static int new_data_fd = 0;
    int map_fd      = 0;

    if (func_init)
    {
        old_data_fd = open_file_in_tmp_dir(STANDARD_OLD_DATA_FILE_NAME, tmp_path);
        RETURN_ON_TRUE(old_data_fd == -1, -1);
        new_data_fd = open_file_in_tmp_dir(STANDARD_NEW_DATA_FILE_NAME, tmp_path);
        RETURN_ON_TRUE(new_data_fd == -1, -1);

        func_init = 0;
    }
    
    map_fd = open(path, O_RDONLY);
    RETURN_ON_TRUE(map_fd == -1, -1, perror("couldn't open monitoring file"););

    RETURN_ON_TRUE(ftruncate(old_data_fd, 0) == -1, -1, perror("ftruncate error"););
    RETURN_ON_TRUE(lseek(old_data_fd, SEEK_SET, 0) == -1, -1, perror("lseek error\n"););
    RETURN_ON_TRUE(file_to_file(old_data_fd, new_data_fd) == -1, -1);

    RETURN_ON_TRUE(ftruncate(new_data_fd, 0) == -1, -1, perror("ftruncate error"););
    RETURN_ON_TRUE(lseek(new_data_fd, SEEK_SET, 0) == -1, -1, perror("lseek error\n"););
    RETURN_ON_TRUE(file_to_file(new_data_fd, map_fd) == -1, -1);

    close(map_fd);

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

int file_to_file(const int to_fd, const int from_fd)
{
    LOG("-------------------file to file converting-------------------------\n");

    LOG("> file descriptors are: %d and %d\n", to_fd, from_fd);

    off_t file_size = get_file_size(from_fd);
    RETURN_ON_TRUE(file_size == -1, -1);
    RETURN_ON_TRUE(file_size == 0, 0);

    LOG("> file size is %ld\n", file_size);

    char *buff = (char *)calloc((size_t)file_size, sizeof(char));
    RETURN_ON_TRUE(!buff, -1, 
        printf("processmon: buffer memory allocation error\n");
        close(to_fd);
        close(from_fd););

    ssize_t bytes_read = read(from_fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file read error");
        close(to_fd);
        close(from_fd););

    LOG("> %ld bytes were read to buff\n", bytes_read);

    bytes_read = write(to_fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file write error");
        close(to_fd);
        close(from_fd););

    LOG("> %ld bytes were written from buff\n", bytes_read);

    free(buff);

    return 0;
}

off_t get_file_size(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    RETURN_ON_TRUE(file_size == -1, -1,
        perror("file size detection error"););
    lseek(fd, 0, SEEK_SET);

    LOG("> file size = %ld\n", file_size);

    return file_size;
}