#ifndef CMD_PARSER
#define CMD_PARSER

#define _POS_SHIFT(num)                                                                     \
    (*str_pos) += num;                                                                      \
    LOG("current position is <%d>, the rest of the string is <%s> ", *str_pos, str + *str_pos) 


char ***parse_string(char *string, unsigned int *cmd_num);

void clear_buffers(char ***cmd_arr, const unsigned int cmd_arr_size);

#endif