#ifndef CONFIG_STRUCTURE
#define CONFIG_STRUCTURE

#include <sys/types.h>

typedef struct 
{
    int             monitoring_pid;
    unsigned int    period;
    char            *output_file_path;
    char            *tmp_folder_path;
    char            tmp_delete_bool;
} config_st;

int init_cfg_struct(config_st *cfg);

void destr_cfg_struct(config_st *cfg);

#endif