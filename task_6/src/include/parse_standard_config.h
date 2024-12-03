#ifndef STANDARD_CONFIG_LOAD
#define STANDARD_CONFIG_LOAD

typedef struct 
{
    pid_t           monitoring_pid;
    unsigned int    period;
    int             diff_file_fd;
} config_st;

//Standard config is located in binary directory
#define STANDARD_CONFIG_NAME    "%s/standard_config.cfg"


#define STANDARD_PERIOD         5
#define STANDARD_FILE_OUTPUT    "./output.txt"
#define STANDARD_TMP_PATH       "./processmon_tmp"


#define CONFIG_FILE_MAX_SIZE    1024

#define CONFIG_FILE_STRUCTURE   \
"PERIOD: %d\n\
OUTPUT PATH: %s\n\
TEMP FOLDER PATH: %s"

int load_standard_config(config_st *cfg);

#endif