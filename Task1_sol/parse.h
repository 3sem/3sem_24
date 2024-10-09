#ifndef PARSE
#define PARSE

#define CMD_MAX_SIZE 4096

char*** parse_buffer(char* buf);
int count_symbol(char* str, const char* sym);
void free_cmd(char*** cmd_data, char* buf);
int count_args(char *cmd);


#endif