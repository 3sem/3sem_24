#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.h"


char*** parse_buffer(char* buf){
    char pipe_sep[] = "|";

    int cmd_counter = count_symbol(buf, pipe_sep) + 1;

    char** cmd_data = (char**)calloc(cmd_counter, sizeof(char*)); //делаем массив для хранения комманд разделенных пайпами

    int cnt = 0;

    char* istr = 0;
    istr = strtok(buf, pipe_sep); //заполняем массив команд

    while (istr != NULL){
        cmd_data[cnt] = istr;
        istr = strtok(NULL, pipe_sep);
        cnt++;
    }


    cnt = 0; 
    const char* arg_sep = " \n";
    int args_num = 0;

    char*** arg_data = (char***)calloc(cmd_counter + 1, sizeof(char**)); //делаем массив аргументов
    

    while (cnt < cmd_counter){                                      //теперь токенизируем каждый эдемент двумерного массива и заполняем таким образом трехмерный
        args_num = count_args(cmd_data[cnt]);
        
        arg_data[cnt] = (char**)calloc(args_num + 1, sizeof(char*));
        istr = strtok(cmd_data[cnt], arg_sep);

        int i = 0;
        
        while (istr != NULL){
            arg_data[cnt][i] = istr;
            istr = strtok(NULL, arg_sep);
            i++;
        }

        cnt++;
       
    }
    
    free(cmd_data);

    arg_data[cnt] = NULL;
 
    return arg_data;
    
    
}


int count_symbol(char* str, const char* sym){   //не передаю char sym, чтобы корректно работал strtok
    int cntr = 0;
    int i = 0;

    while (str[i] != '\0'){
        if (str[i] == sym[0]) cntr++;
        i++;
    }
    return cntr; 
}

int count_args(char *cmd){                      //подсчет аргументов в каждой команде
    int cntr = 0;
    char cmd_buf[CMD_MAX_SIZE] = {0};

    for (int pos = 0, n = 0; (pos < strlen(cmd)); )
    {
        if (sscanf(cmd + pos, "%[^ \n]%n", cmd_buf, &n))
        {
            pos += n;
            cntr++;

            continue;
        }

        pos++;
    }

    return cntr;
}

void free_cmd(char*** cmd_data, char* buf){         //очищаем всю аллоцированную память
    int pipe_cnt = count_symbol(buf, "|") + 1; 

    int i = 0;
    while (i < pipe_cnt){
        free(cmd_data[i]);
        i++;
    }

    free(cmd_data);
}