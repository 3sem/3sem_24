#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "socket.h"


Socket* create_socket()
{
	Socket* const socket = (Socket*)calloc( 1, sizeof(Socket));

	if ( socket == NULL )
	{
		return NULL;
	}

	socket->buffer = (char*)calloc( SOCKET_BUFFER_SIZE, sizeof(char));
	if ( socket->buffer == NULL )
	{
		free( socket);

		return NULL;
	}
	
	pipe( socket->forward);
	pipe( socket->backward);

	return socket;
}

void destroy_socket( Socket *const socket)
{
	close( socket->forward[0]);
	close( socket->forward[1]);

	close( socket->backward[0]);
	close( socket->backward[1]);

	free( socket->buffer);
	free( socket);
}

ssize_t write_socket( Socket *const socket,
                      const SocketDir direction)
{
	int fd = 0;

	switch ( direction )
	{
	    case DIR_FW:
		    fd = socket->forward[1];
		    break;

	    case DIR_BW:
		    fd = socket->backward[1];
		    break;

	    default:
            printf( "Inccorect direction = %d\n",
                    direction);
		    break;
	}

	return write( fd, socket->buffer, socket->length);
}

ssize_t read_socket( Socket *const socket,
                     const SocketDir direction)
{	
	int fd = 0;

	switch ( direction )
	{
	    case DIR_FW:
            fd = socket->forward[0];
            break;

	    case DIR_BW:
            fd = socket->backward[0];
            break;

	    default:
            printf( "incorrect direction = %d",
                    direction);
            break;
	}

	return socket->length = read( fd, socket->buffer, SOCKET_BUFFER_SIZE);
}

void close_socket( const Socket *const socket,
                   const SocketDir direction)
{
	switch ( direction )
	{
        case DIR_FW:
            close( socket->forward [0]);
            close( socket->backward[1]);
            break;
            
        case DIR_BW:
            close( socket->forward [1]);
            close( socket->backward[0]);
            break;

        default:
            printf( "incorrect direction = %d",
                    direction);
            break;	
	}
}
