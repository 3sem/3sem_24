#pragma once

#include <time.h>

double getTIME(int(*filetransfer)(int, int, struct timespec*, struct timespec*));
int timeShMem(int fread, int fwrite, struct timespec* start, struct timespec* end);
int timeFIFO(int fread, int fwrite, struct timespec* start, struct timespec* end);
int timeQueue(int fread, int fwrite, struct timespec* start, struct timespec* end);