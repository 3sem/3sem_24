#ifndef INTERFACE_COMMANDS
#define INTERFACE_COMMANDS

int read_number_from_input();

int change_pid(const int fd);

int change_period(const int fd);

int change_tmp_directory(const int fd);

int end_programm(const int child_pid);

#endif