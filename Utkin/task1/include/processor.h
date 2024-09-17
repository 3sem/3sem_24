#ifndef PROCESSOR_H
#define PROCESSOR_H

#define MAX_LENGHT 1024

void process_child(char* cmd, char** args, int* pipe0, int pipe_fd[2], int num_cmds, int current_cmd);
int processed(char* mem);

#endif
