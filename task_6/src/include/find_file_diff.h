#ifndef FIND_DIFF_IN_FILE
#define FIND_DIFF_IN_FILE

#include "tmp_dir_st.h"

#define MAPS_DIR    "/proc/%d/maps"

int file_diff(const int monitoring_pid, tmp_st *dir_st);

#endif