#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"

static int command_ctor(Command* command, char** command_text, size_t argument_count, CommandsSplitter splitter, Command* next, Command* prev);
static int command_dtor(Command* command);
static void command_dump(const Command* command);
static void int_swap(int* first, int* second);

//-------------------------------> COMMAND ARRAY FUNCTION <-------------------------------------//

int commands_array_ctor(CommandsArray* commands_array, size_t commands_count) {
    assert(commands_array);

    commands_array->array = (Command*) calloc(commands_count, sizeof(Command));
    if (!(commands_array->array)) return 1;

    commands_array->commands_capacity = commands_count;
    commands_array->commands_count    = 0;

    return 0;
}

int commands_array_dtor(CommandsArray* commands_array) {
    assert(commands_array);

    for (size_t i = 0; i < commands_array->commands_count; i++) {
        command_dtor(&(commands_array->array[i]));
    }

    commands_array->commands_capacity = 0;
    commands_array->commands_count    = 0;

    free(commands_array->array);
    commands_array->array = NULL;

    return 0;
}

int commands_array_add(CommandsArray* commands_array, char** command, size_t argument_count, CommandsSplitter splitter) {
    assert(commands_array);
    assert(command);

    Command* prev_command = NULL;   
    if (commands_array->commands_count) {
        prev_command = &(commands_array->array[commands_array->commands_count - 1]);
    }

    command_ctor(&(commands_array->array[commands_array->commands_count]), command, argument_count, splitter, NULL, prev_command);

    if (prev_command) {
        prev_command->next = &(commands_array->array[commands_array->commands_count]);
    }

    (commands_array->commands_count)++;

    return 0;
}

void commands_array_dump(const CommandsArray* commands_array) {
    assert(commands_array);

    printf( "COMMAND COUNT: %lu\n"
            "COMMAND CAPACITY: %lu\n"
            "COMMANDS:\n\n", commands_array->commands_count, commands_array->commands_capacity);

    for (size_t i = 0; i < commands_array->commands_count; i++) {
        command_dump(&(commands_array->array[i]));
    }
}

//-----------------------------------> COMMAND FUNCTIONS <------------------------------//

CommandsSplitter get_prev_splitter(Command* command) {
    assert(command);

    if (command->prev) return command->prev->splitter;

    return NO_COMMAND_SPLITTER;
}

CommandsSplitter get_splitter(Command* command) {
    assert(command);
    return command->splitter;
}

static void int_swap(int* first, int* second) {
    assert(first);
    assert(second);

    int temp = *first;
    *first   = *second;
    *second  = temp;
}

void pipe_swap(int first[2], int second[2]) {
    assert(first);
    assert(second);

    int_swap(&(first[0]), &(second[0]));
    int_swap(&(first[1]), &(second[1]));
}

static int command_ctor(Command* command, char** command_text, size_t argument_count, CommandsSplitter splitter, Command* next, Command* prev) {
    assert(command);
    assert(command_text);

    command->argument_count = argument_count;   // initialize command
    command->command        = command_text[0];
    command->splitter       = splitter;
    command->next           = next;
    command->prev           = prev;

    command->argument_list = (char**) calloc(argument_count + 1, sizeof(char*));
    if (!(command->argument_list)) return 1;

    for (size_t i = 0; i < argument_count; i++) {
        command->argument_list[i] = command_text[i+1];
    }

    // command->argument_list[argument_count] = NULL; // Make null terminated argument array

    return 0;
}

static int command_dtor(Command* command) {
    assert(command);

    free(command->argument_list);
    command->argument_count = 0;
    command->command        = NULL;
    command->splitter       = NO_COMMAND_SPLITTER;
    command->next           = NULL;
    command->prev           = NULL;

    return 0;
}

static void command_dump(const Command* command) {
    assert(command);

    printf("ADDRESS: %p\n",          command);
    printf("COMMAND: %s\n",          command->command);
    printf("ARG COUNT: %lu\n",       command->argument_count);
    printf("COMMAND SPLITTER: %u\n", command->splitter);
    printf("NEXT: %p\n",             command->next);
    printf("PREV: %p\n",             command->prev);
    printf("ARGS: ");

    for (size_t i = 0; i < command->argument_count; i++) {
        printf("%s ", command->argument_list[i]);
    }
    printf("\n\n");
}