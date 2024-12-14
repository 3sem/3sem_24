#ifndef SOCKET_H
#define SOCKET_H

#define SOCKET_BUFFER_SIZE 4096

#include <stdlib.h>

typedef struct Socket
{
	int forward [2];
	int backward[2];
	
	char   *buffer;
	size_t  length;
} Socket;

typedef enum SocketDir
{
	DIR_FW = 0,
	DIR_BW = 1
} SocketDir;

Socket* create_socket ( void);
void    destroy_socket( Socket *const socket);

ssize_t write_socket( Socket *const socket, const SocketDir direction);
ssize_t read_socket ( Socket *const socket, const SocketDir direction);

void close_socket( const Socket *const socket, const SocketDir direction);

#endif // SOCKET_H
