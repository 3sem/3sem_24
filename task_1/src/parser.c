#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "parser.h"

const char* AND_COMMAND_TOKEN  = "&&";
const char* PIPE_COMMAND_TOKEN = "|";

typedef struct {
    size_t           start;
    size_t           end;
    CommandsSplitter splitter;
} Pair;

typedef struct {
    Pair*   array;
    size_t  capacity;
    size_t  current_pointer;
} StartPositionArray;

static StartPositionArray start_position_array_ctor(size_t capacity);
static int start_position_array_dtor(StartPositionArray* array);

// TODO make full version with many commands support
int parse_commands(size_t argc, char* argv[], CommandsArray* commands_array) {
    #define ADD_SPLITTER_HANDLING(__TOKEN__, __SPLITTER__) do {                                                 \
        if (!strcmp(argv[i], __TOKEN__)) {                                                                      \
            command_start_positions.array[command_start_positions.current_pointer].end = i - 1;                 \
            command_start_positions.array[command_start_positions.current_pointer].splitter = __SPLITTER__;     \
                                                                                                                \
            command_start_positions.current_pointer++;                                                          \
            command_start_positions.array[command_start_positions.current_pointer].start = i + 1;               \
        }                                                                                                       \
    } while(0)                                                                                                  \

    assert(argv);
    assert(commands_array);
    
    if (argc < 1) return 0;

    StartPositionArray command_start_positions = start_position_array_ctor(argc);

    command_start_positions.array[command_start_positions.current_pointer].start    = 0;  
    command_start_positions.array[command_start_positions.current_pointer].splitter = NO_COMMAND_SPLITTER;
    for (size_t i = 0; i < argc; i++) {
        ADD_SPLITTER_HANDLING(AND_COMMAND_TOKEN,  AND_COMMAND_SPLITTER);
        ADD_SPLITTER_HANDLING(PIPE_COMMAND_TOKEN, PIPE_COMMAND_SPLITTER);
    }
    command_start_positions.array[command_start_positions.current_pointer].end = argc - 1;
    command_start_positions.current_pointer++;

    if (commands_array_ctor(commands_array, command_start_positions.current_pointer)) {
        assert(0);  // TODO add normal handling of commands array ctor error
    }

    for (size_t i = 0; i < command_start_positions.current_pointer; i++) {
        commands_array_add( commands_array,
                            argv + command_start_positions.array[i].start, 
                            command_start_positions.array[i].end - command_start_positions.array[i].start,
                            command_start_positions.array[i].splitter);
    }

    start_position_array_dtor(&command_start_positions);

    return 0;

    #undef ADD_SPLITTER_HANDLING
}

static StartPositionArray start_position_array_ctor(size_t capacity) {
    StartPositionArray array = {};
    
    array.array = (Pair*) calloc(capacity, sizeof(Pair));
    assert(array.array);

    array.capacity        = capacity;
    array.current_pointer = 0;

    return array;
}

static int start_position_array_dtor(StartPositionArray* array) {
    assert(array);

    free(array->array);
    array->capacity = 0;

    return 0;
}   