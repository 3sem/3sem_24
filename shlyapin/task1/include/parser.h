#ifndef PARSER_H
#define PARSER_H

char **parse_cmd(const char *cmd_ptr, const char *sep);
void free_parsed_cmd(char **args);
int count_char(const char *str, char chr);

#endif // PARSER_H
