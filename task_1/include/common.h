#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

typedef enum {
    NO_COMMAND_SPLITTER,         
    AND_COMMAND_SPLITTER,        
    PIPE_COMMAND_SPLITTER,       
    BACKGROUND_COMMAND_SPLITTER
} CommandsSplitter;

typedef struct __Command__ {
    char*               command;
    size_t              argument_count;
    char**              argument_list;
    CommandsSplitter    splitter;
    struct __Command__* next;
    struct __Command__* prev;
} Command;

typedef struct {
    Command*       array;
    size_t         commands_count;
    size_t         commands_capacity;
} CommandsArray;

int commands_array_ctor(CommandsArray* commands_array, size_t commands_count);
int commands_array_dtor(CommandsArray* commands_array);
int commands_array_add(CommandsArray* commands_array, char** command, size_t argument_count, CommandsSplitter splitter);
void commands_array_dump(const CommandsArray* commands_array);

CommandsSplitter get_prev_splitter(Command* command);
CommandsSplitter get_splitter(Command* command);
void pipe_swap(int first[2], int second[2]);

#endif