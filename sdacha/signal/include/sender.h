#ifndef SENDER_H
#define SENDER_H


#include <unistd.h>


int sender( const pid_t pid, const char *const log_path,
            const int n_pages, const int times);


#endif // SENDER_H
