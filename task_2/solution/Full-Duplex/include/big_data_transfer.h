#ifndef BIG_DATA_TRANSFER_H
#define BIG_DATA_TRANSFER_H

#include <stdbool.h>

typedef struct pPipe Pipe;

void send_big_data  (const char* file_name, Pipe* pipe, bool is_parent);

void recieve_big_data(const char* file_name, Pipe* pipe, bool is_parent);

#endif // BIG_DATA_TRANSFER_H
