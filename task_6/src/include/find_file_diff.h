#ifndef FIND_DIFF_IN_FILE
#define FIND_DIFF_IN_FILE

#define STANDARD_DIFF_OUTPUT_DIR    "./processmon/%d.diff"
#define STANDARD_OLD_DATA_FILE_PATH "./processmon/old_data.txt"
#define STANDARD_NEW_DATA_FILE_PATH "./processmon/new_data.txt"

#define STANDARD_TMP_DIR            "./processmon/"


int file_diff(const char *path);

int create_tmp_dir();

void clear_tmp();

#endif