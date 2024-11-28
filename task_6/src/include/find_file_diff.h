#ifndef FIND_DIFF_IN_FILE
#define FIND_DIFF_IN_FILE

#define STANDARD_DIFF_OUTPUT_DIR "/tmp/%d.diff"

int find_file_diff(const char *path);

void clear_all();

#endif