#pragma once

#include "fdp_pipe.h"

size_t parent_send(Pipe* self);
size_t child_send(Pipe* self);

size_t parent_recieve(Pipe* self);
size_t child_recieve(Pipe* self);

size_t get_file_size(const char* filename);