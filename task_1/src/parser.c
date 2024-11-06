#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "log.h"

/*
    Синтаксис

    pipe    := cmd {'|' cmd}
    cmd     := arg {' ,' arg}
    arg     := ![| ,\n]
*/

char ***pipe_parsing(char *str, int *str_pos, const size_t num_of_pipes);

char **cmd_parsing(char *str, int *str_pos);

char *arg_parsing(char *str, int *str_pos);

size_t get_arg_number(const char *string);

size_t get_pipe_number(char *string);


char ***parse_string(char *string, unsigned int *cmd_num)
{
    assert(string);
    assert(cmd_num);

    LOG("starting string parsing");
    LOG("getting number of pipes:");
    size_t pipe_number = get_pipe_number(string);
    if (!pipe_number)
    {
        LOG_ERR("pipe number couldn't be zero");
        return 0;
    }

    LOG("total number of pipes is: %d", pipe_number);
    LOG("parsing pipes:");

    int str_pos = 0;
    char ***commands = pipe_parsing(string, &str_pos, pipe_number);
    *cmd_num = (unsigned int)pipe_number;

    return commands;
}

char ***pipe_parsing(char *str, int *str_pos, const size_t num_of_pipes)
{
    assert(str);
    assert(str_pos);

    char ***cmd_arr = (char ***)calloc(num_of_pipes + 1, sizeof(char **));
    if (!cmd_arr)
    {
        LOG_ERR("couldn't allocate memory for the commands array");
        return NULL;
    }
    
    unsigned int pipe_number = 0;
    char **cmd = cmd_parsing(str, str_pos);
    if (!cmd)
    {
        free(cmd_arr);
        return NULL;
    }

    cmd_arr[pipe_number] = cmd;
    pipe_number++;
    
    while (str[*str_pos] == '|')
    {
        str[*str_pos] = '\0';
        _POS_SHIFT(1);

        cmd = cmd_parsing(str, str_pos);
        if (!cmd)
        {
            clear_buffers(cmd_arr, pipe_number);
            return NULL;
        }

        cmd_arr[pipe_number] = cmd;
        pipe_number++;
    }

    if (pipe_number != num_of_pipes)
    {
        LOG_ERR("number of commands parsed does not match with the number of commands expected");
        free(cmd_arr);
        return NULL;
    }
    
    return cmd_arr;
}

char **cmd_parsing(char *str, int *str_pos)
{
    assert(str);
    assert(str_pos);

    size_t num_of_arguments = get_arg_number(str + *str_pos);
    if (!num_of_arguments)
    {
        LOG_ERR("arg number couldn't equal zero");
        return NULL;
    }
    
    char **arg_arr = (char **)calloc(num_of_arguments + 1, sizeof(char *));
    if (!arg_arr)
    {
        LOG_ERR("couldn't allocate memory for the arguments array");
        return NULL;
    }

    LOG("parsing command:");

    char *arg = NULL;
    unsigned int arg_num = 0;
    do
    {
        while (str[*str_pos] == ' ' || str[*str_pos] == ',')
            (*str_pos)++;
        
        arg = arg_parsing(str, str_pos);
        if (arg)
        {
            arg_arr[arg_num] = arg;
            arg_num++;
        }
    } while (arg != NULL);

    if (arg_num != num_of_arguments)
    {
        LOG_ERR("number of arguments scanned {%d} does not match with the number of arguments expected {%d}", arg_num, num_of_arguments);
        free(arg_arr);
        return NULL;
    }

    LOG("command parsed");

    LOG("scanned command arguments are:");
    for (unsigned int n = 0; n < arg_num; n++)
    {
        LOG("%d) %s", n, arg_arr[n]);
    }
    
    return arg_arr;
}

char *arg_parsing(char *str, int *str_pos)
{
    assert(str);
    assert(str_pos);

    char cmd_holder[_POSIX_ARG_MAX] = {0};

    LOG("parsing argument: ");

    int n = 0;
    sscanf(str + *str_pos, "%[^ ,\n|]%n", cmd_holder, &n);
    if (n)
    {
        char *arg = str + *str_pos;
        _POS_SHIFT(n);

        LOG("number of symbols scanned is: <%d>, the argument scanned is: <%s>", n, cmd_holder);

        if (str[*str_pos] != '|' && str[*str_pos] != '\0')
        {
            str[*str_pos] = '\0';
            (*str_pos)++;
        }

        return arg;
    }
    
    return NULL;
}

size_t get_arg_number(const char *string)
{
    assert(string);

    size_t cmd_number = 0;
    char cmd_holder[_POSIX_ARG_MAX] = {0};

    for (int pos = 0, n = 0; (string[pos] != '|') && (pos < (int)strlen(string)); )
    {
        if (sscanf(string + pos, "%[^ ,\n|]%n", cmd_holder, &n))
        {
            pos += n;
            cmd_number++;

            continue;
        }

        pos++;
    }
    
    return cmd_number;
}

size_t get_pipe_number(char *string)
{
    assert(string);

    size_t pipe_number = 0;
    for (const char *pipe_pos = string;; pipe_pos = strchr(pipe_pos, '|'))
    {
        if (!pipe_pos)
            break;

        LOG("pipe number is: %d", pipe_number);
        pipe_number++;

        pipe_pos++;
    }
    
    return pipe_number;
}

void clear_buffers(char ***cmd_arr, const unsigned int cmd_arr_size)
{
    assert(cmd_arr);

    for (unsigned int cmd = 0; cmd < cmd_arr_size; cmd++)
        free(cmd_arr[cmd]);

    free(cmd_arr);
    return;
}