#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include "config_structure.h"
#include "debugging.h"

int init_cfg_struct(config_st *cfg)
{
    assert(cfg);

    cfg->monitoring_pid = -1;
    char *path_one = (char *)calloc(PATH_MAX, sizeof(char));
    RETURN_ON_TRUE(!path_one, MEM_ALC_ERR);

    char *path_two = (char *)calloc(PATH_MAX, sizeof(char));
    RETURN_ON_TRUE(!path_two, MEM_ALC_ERR);

    cfg->output_file_path = path_one;
    cfg->tmp_folder_path  = path_two;

    return 0;
}

void destr_cfg_struct(config_st *cfg)
{
    assert(cfg);

    cfg->monitoring_pid     = 0;
    cfg->period             = 0;
    cfg->tmp_delete_bool    = 0;
    
    free(cfg->output_file_path);
    free(cfg->tmp_folder_path);

    cfg->output_file_path   = NULL;
    cfg->tmp_folder_path    = NULL;

    return;
}

