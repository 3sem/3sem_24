#ifndef RECEIVE_SEND_H
#define RECEIVE_SEND_H

#include <stddef.h>
#include <duplex_pipe.h>

size_t childReceive  (Pipe* dpipe);

size_t parentReceive (Pipe* dpipe);

size_t parentSend    (Pipe* dpipe);

size_t childSend     (Pipe* dpipe);

#endif // RECEIVE_SEND_H
