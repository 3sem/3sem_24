#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <assert.h>
#include <memory.h>
#include "debugging.h"
#include "find_file_diff.h"

static char *curr_file_path  = NULL;

off_t get_file_size(int fd);

int write_first_diff(const char *path);

int two_file_diff(const char* map_file_path, const int diff_counter);


int find_file_diff(const char *path)
{
    assert(path);

    static int   diff_counter = 0;
    int          map_fd       = 0;

    if (curr_file_path == 0)
    {
        map_fd = write_first_diff(path);
        RETURN_ON_TRUE(map_fd == -1, -1, free(curr_file_path););

        diff_counter++;
        return 0;
    }

    
    int err_num = two_file_diff(path, diff_counter);
    RETURN_ON_TRUE(err_num == -1, -1,
        close(map_fd);
        free(curr_file_path););
    
    diff_counter++;

    return 0;
}

int write_first_diff(const char *path)
{
    assert(path);

    char *output_path = (char *)calloc(PATH_MAX, sizeof(char));
    RETURN_ON_TRUE(!output_path, -1, printf("processmon: output path string calloc error"););
    snprintf(output_path, PATH_MAX * sizeof(char), STANDARD_DIFF_OUTPUT_DIR, 0);

    int fd = open(path, O_RDONLY);
    RETURN_ON_TRUE(fd == -1, -1, perror("first time map opening error"););

    int output_fd = open(output_path, O_RDWR | O_CREAT, 0777);
    RETURN_ON_TRUE(output_fd == -1, -1, 
        perror("diff file opening error"); 
        close(fd););

    off_t file_size = get_file_size(fd);
    RETURN_ON_TRUE(file_size == -1, -1);
    char *buff = (char *)calloc((size_t)file_size, sizeof(char));
    RETURN_ON_TRUE(!buff, -1, 
        printf("processmon: buffer memory allocation error\n");
        close(fd);
        close(output_fd););

    ssize_t bytes_read = read(fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file read error");
        close(fd);
        close(output_fd);
        free(buff););

    bytes_read = write(output_fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file write error");
        close(fd);
        close(output_fd);
        free(buff););

    curr_file_path = output_path;

    free(buff);

    return fd;
}

int two_file_diff(const char* map_file_path, const int diff_counter)
{
    assert(curr_file_path);
    assert(map_file_path);

    //Старый файл сохраняется в НОВЫЙ ВРЕМЕННЫЙ, НЕ DIFF ФАЙЛ

    char old_file[PATH_MAX] = {};
    memcpy(old_file, curr_file_path, PATH_MAX * sizeof(char));

    snprintf(curr_file_path, PATH_MAX * sizeof(char), STANDARD_DIFF_OUTPUT_DIR, diff_counter);

    pid_t pid = fork();
    RETURN_ON_TRUE(pid == -1, -1, perror("pid error"););

    if (pid)
        return 0;

    printf("difference between \"%s\" and \"%s\" output path is: %s\n", map_file_path, old_file, curr_file_path);
    int fd = open(old_file, O_CREAT | O_WRONLY, 0777);
    printf("fd is %d\n", fd);
    RETURN_ON_TRUE(fd == -1, -1, perror("diff file opening error"););
    
    dup2(fd, STDOUT_FILENO);
    //close(STDOUT_FILENO);

    int err_num = execl("/bin/diff", "diff", old_file, map_file_path, (char *)NULL);
    RETURN_ON_TRUE(err_num == -1, -1, 
        perror("execl error");
        close(fd););

    close(fd);

    exit(1);
}

off_t get_file_size(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    RETURN_ON_TRUE(file_size == -1, -1,
        perror("file size detection error"););
    lseek(fd, 0, SEEK_SET);

    return file_size;
}

void clear_all()
{
    free(curr_file_path);
}