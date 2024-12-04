#ifndef FIND_DIFF_IN_FILE
#define FIND_DIFF_IN_FILE

#define STANDARD_DIFF_OUTPUT_FORMAT ".diff"
#define STANDARD_OLD_DATA_FILE_NAME "old_data.txt"
#define STANDARD_NEW_DATA_FILE_NAME "new_data.txt"

int file_diff(const char *path, const char *cfg);

off_t get_file_size(int fd);

int create_tmp_dir(const char *path);

void clear_tmp(const char *tmp_path);

#endif