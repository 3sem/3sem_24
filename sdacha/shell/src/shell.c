#include "token.h"
#include "shell.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

static const char* PIPES[] = { "PIPE_1", "PIPE_2" };


static void
unlink_pipes()
{
    unlink( PIPES[0]);
    unlink( PIPES[1]);
}

CmdArray
input_cmds()
{
    static char buffer[MAX_STR_SIZE] = { 0 };

    int sym = 0;
    int i   = 0;

    do
    {
        sym = getchar();
        buffer[i] = sym;
        i++;
        
    } while ( sym != '\n' && sym != EOF && i < MAX_STR_SIZE );
    
    return parse_cmds( buffer);
    
}

static void
make_pipes( void)
{
    mkfifo( PIPES[0], S_IRUSR | S_IWUSR);
    mkfifo( PIPES[1], S_IRUSR | S_IWUSR);
}

#define CMD cmd_arr.cmds
#define CUR_PIPE PIPES[cmd_count % 2]
#define EXEC(cmd_count) \
    execvp( CMD[cmd_count].exec_args[0], CMD[cmd_count].exec_args)

int*
executer( CmdArray cmd_arr)
{
    int* const exit_codes = (int*)calloc( cmd_arr.n_cmds, sizeof(int));

    if ( exit_codes == NULL )
    {
        return NULL;
    }
    
    if ( cmd_arr.n_cmds <= 0 )
    {
        return NULL;
    }

    pid_t pid = 0;

    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;
    int cmd_count = 0;
    int wstatus = 0;
    int pipe_need = cmd_arr.n_cmds >= 2;

    if ( pipe_need )
    {
        
        make_pipes();
        pid = fork();


        if ( pid == 0 )
        {
            fd_out = open( CUR_PIPE, O_WRONLY);
            dup2( fd_out, STDOUT_FILENO);

            EXEC( cmd_count);
        }

        fd_in = open( CUR_PIPE, O_RDONLY);

        wait( &wstatus);
        exit_codes[0] = WEXITSTATUS( wstatus);

        for ( cmd_count = 1; cmd_count < cmd_arr.n_cmds - 1; cmd_count++ ) 
        { 
            pid = fork();

            if ( pid == 0 )
            {
                fd_out = open( CUR_PIPE, O_WRONLY);
                dup2( fd_in, STDIN_FILENO);
                dup2( fd_out, STDOUT_FILENO);

                EXEC( cmd_count);
            }

            fd_in = open( CUR_PIPE, O_RDONLY);

            wait( &wstatus);
            exit_codes[cmd_count] = WEXITSTATUS( wstatus);
        }
    }

    pid = fork();

    if ( pid == 0 )
    {
        dup2( fd_in, STDIN_FILENO);

        EXEC( cmd_count);
    }

    wait( &wstatus);
    exit_codes[cmd_arr.n_cmds - 1] = WEXITSTATUS( wstatus);

    if ( pipe_need )
    {
        unlink_pipes();
    }

    return exit_codes;

}


void
print_exit_codes( int* exit_codes,
                  CmdArray cmd_array)
{
    printf( "[");
    
    int i = 0;
    for ( ; i < cmd_array.n_cmds - 1; ++i )
    {
        printf( "%d|", exit_codes[i]);
    }

    printf( "%d]", exit_codes[i]);
}
