#ifndef STANDARD_CONFIG_LOAD
#define STANDARD_CONFIG_LOAD

#include "config_structure.h"

//Standard config is located in programm's binary directory
#define STANDARD_CONFIG_NAME    "%s/standard_config.cfg"


#define STANDARD_PERIOD         (unsigned int)5
#define STANDARD_FILE_OUTPUT    "./output.txt"
#define STANDARD_TMP_PATH       "/tmp/processmon_data"
#define STANDARD_TMP_DEL_BOOL   1


#define CONFIG_FILE_MAX_SIZE    1024

#define MON_PERIOD  "PERIOD: %u\n"
#define OUT_PATH    "OUTPUT PATH: %s\n"
#define TMP_PATH    "TEMP FOLDER PATH: %s\n"
#define TMP_DEL     "DELETE TEMP FOLDER AFTER PROGRAMM IS FINISHED: %d"

#define CONFIG_FILE_STRUCTURE   \
MON_PERIOD                      \
OUT_PATH                        \
TMP_PATH                        \
TMP_DEL

int load_standard_config(config_st *cfg);

int save_current_config(const config_st *cfg);

void print_current_parameters(const config_st *cfg);

#endif