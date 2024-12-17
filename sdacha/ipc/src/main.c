#include "ipc.h"
#include <stdio.h>


int main( int argc, char* argv[])
{
    if ( argc > 2 )
    {
        printf( "Arg num error\n");
    }

    //test_shm( argv[1]);
    //test_pipe( argv[1]);
    test_msg( argv[1]);
}
