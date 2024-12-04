#define _GNU_SOURCE

#include <stdio.h>
#include <ftw.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include "debugging.h"
#include <change_tmp_dir.h>

static const char *new_path = NULL;

static char dir_changing_status = 0;


void set_changing_status_true();

void set_changing_status_false();

int move_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf);


void set_changing_status_true()
{
    dir_changing_status = 1;
}

void set_changing_status_false()
{
    dir_changing_status = 0;
}

char check_tmp_changing_status()
{
    return dir_changing_status;
}

int move_tmp_dir(const char *path_to, const char *old_path)
{
    assert(path_to);

    new_path = path_to;

    set_changing_status_true();
    int ret_val = nftw(old_path, move_file, 10, FTW_CHDIR | FTW_DEPTH);
    set_changing_status_false();    

    new_path = NULL;
    return ret_val;
}


//ПЕРЕДЕЛАТЬ функцию под для нормальной работы
int move_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    (void)sb;
    (void)typeflag;
    (void)ftwbuf;

    printf("> %s\n", path);

    if (typeflag == FTW_D)
        RETURN_ON_TRUE(mkdir(new_path, S_IFDIR | 0777) == -1, -1, perror("couldn't create directory"););
    
    int ret_val = 0;
    if (typeflag == FTW_F)
    {
        char file_path[PATH_MAX] = {};
        snprintf(file_path, PATH_MAX * sizeof(char), "%s%s", new_path, path);
        printf(">> %s\n", file_path);

        ret_val = rename(path, file_path);       
        printf("ret val = %d, errno = %d, path = %s\n", ret_val, errno, path);
    }
    
    return ret_val;
}