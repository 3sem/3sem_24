#define _GNU_SOURCE

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>
#include <fcntl.h>
#include <limits.h>
#include <assert.h>
#include <memory.h>
#include "debugging.h"
#include "find_file_diff.h"


static int diff_fd = 0;


int delete_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int write_diff();

int open_diff_file(const int file_number);

int check_file_emptyness(int fd);

int prepare_file_diff(const char *path);

int file_to_file(const char *to_file, const char *from_file);

off_t get_file_size(int fd);


int file_diff(const char *path)
{
    assert(path);
    
    int err_num = prepare_file_diff(path);
    RETURN_ON_TRUE(err_num == -1, -1);

    err_num = write_diff();
    RETURN_ON_TRUE(err_num == -1, -1);

    return 0;
}

int create_tmp_dir()
{
    return mkdir(STANDARD_TMP_DIR, S_IFDIR | 0777);
}

void clear_tmp()
{
    RETURN_ON_TRUE(nftw(STANDARD_TMP_DIR, delete_file, 10, FTW_DEPTH | FTW_PHYS) == -1,, printf("Processmon: error occured while deleting temporary files\n"););
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
int write_diff()
{
    static int diff_counter = 0;

    if (diff_fd == 0)
    {
        diff_fd = open_diff_file(diff_counter);
        RETURN_ON_TRUE(diff_fd == -1, -1);
    }

    if (check_file_emptyness(diff_fd))
    {
        close(diff_fd);
        diff_counter++;

        LOG("> sisi\n");

        diff_fd = open_diff_file(diff_counter);
        RETURN_ON_TRUE(diff_fd == -1, -1);
    }
    
    pid_t pid = fork();
    RETURN_ON_TRUE(pid == -1, -1, perror("pid error"););

    if (pid)
        return 0;

    dup2(diff_fd, STDOUT_FILENO);

    execl("/bin/diff", "diff", STANDARD_OLD_DATA_FILE_PATH, STANDARD_NEW_DATA_FILE_PATH, (char *)NULL);

    return 0;
}

int open_diff_file(const int file_number)
{
    char path[PATH_MAX] = {0};
    snprintf(path, PATH_MAX * sizeof(char), STANDARD_DIFF_OUTPUT_DIR, file_number);

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

int prepare_file_diff(const char *path)
{
    RETURN_ON_TRUE(file_to_file(STANDARD_OLD_DATA_FILE_PATH, STANDARD_NEW_DATA_FILE_PATH) == -1, -1);
    RETURN_ON_TRUE(file_to_file(STANDARD_NEW_DATA_FILE_PATH, path) == -1, -1);

    return 0;
}

int file_to_file(const char *to_file, const char *from_file)
{
    assert(to_file);
    assert(from_file);

    // сделать глобально или статик
    LOG("-------------------file to file converting-------------------------\n");

    int to_fd = open(to_file, O_CREAT | O_WRONLY, 0777);
    RETURN_ON_TRUE(to_fd == -1, -1, perror("file to transfer to opening error"););

    int from_fd = open(from_file, O_CREAT | O_RDONLY, 0777);
    RETURN_ON_TRUE(to_fd == -1, -1, perror("file to transfer from opening error"););

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

    close(to_fd);
    close(from_fd);

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