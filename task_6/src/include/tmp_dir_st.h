#ifndef TMP_DIR_STRUCTURE
#define TMP_DIR_STRUCTURE

typedef struct temp_dir_st tmp_st;
typedef struct op_table Ops;

typedef struct op_table
{
    int (* pid_change_update)(tmp_st *self);
    void (* dir_desctr)(tmp_st *self, const int delete_dir_bool);
    int (* open_new_diff)(tmp_st *self);
    int (* move_tmp_dir)(tmp_st *self, const char *new_path);
} Ops;

typedef struct temp_dir_st
{
    int new_data_fd;
    int old_data_fd;

    long int diff_counter;
    int curr_diff_fd;

    char *tmp_dir;

    Ops methods;
} tmp_st;

#define STANDARD_DIFF_OUTPUT_FORMAT ".diff"
#define STANDARD_OLD_DATA_FILE_NAME "old_data.txt"
#define STANDARD_NEW_DATA_FILE_NAME "new_data.txt"

int tmp_dir_init(tmp_st **tmp, const char *path);

#endif