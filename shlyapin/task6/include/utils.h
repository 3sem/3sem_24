#pragma once

#include <stdlib.h>

static const int TIME_MAX = 256;

int is_type_file(const char *file_path, const char *ftype);
void get_current_time(char *buffer, size_t size);
void get_proc_dir(char *res_path, pid_t pid);
void get_work_dir(char *res_path, pid_t pid);
void change_slash_to_underscore(char *str);
void get_full_path(char *full_path, const char *dir_path, const char *name);
int is_backup(const char *path);
