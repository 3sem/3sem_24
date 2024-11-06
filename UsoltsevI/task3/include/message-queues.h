#pragma once
//-----------------------------------------------------------------
//
// Message Query file sharing header
//
//-----------------------------------------------------------------
//
// msgq_translate_file returns 0 if the process was 
// complited successfully, 1 if an error occured.
//
//-----------------------------------------------------------------

#include <stdio.h>

int msgq_translate_file(int fd, size_t file_size);
