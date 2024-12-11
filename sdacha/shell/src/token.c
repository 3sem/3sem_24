#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "token.h"

#define N_ARGS_DEFAULT -1

CmdError_t
cmd_init( Cmd *self,
          const char **exec_args)
{
    self->exec_args  = exec_args;
    self->n_exec_args = N_ARGS_DEFAULT;
    
    if ( self->exec_args == NULL )
    {
        return MALLOC_ERROR;
    }

    return NO_ERROR;
    
} /* cmd_init */


#define str_pos string->pos

void
skip_spaces( String *string)
{
    while ( isspace( string->str[str_pos]) && str_pos < string->len )
    {
        str_pos++;
    }
} /* skip_spaces */

void
skip_exec_arg( String *string)
{
    while ( !isspace( string->str[str_pos]) && str_pos < string->len )
    {
        str_pos++;
    }
} /* skip_option */

CmdError_t
get_next_pipe( String *string)
{
    int expect_cmd = 0;
    
    if ( str_pos < string->len  && string->str[str_pos] == '|' )
    {
        str_pos++;
        skip_spaces( string);
        expect_cmd = 1;
    }

    // expect cmd, but no one can be parsed
    if ( expect_cmd && str_pos >= string->len )
    {
        return INVALID_PIPE; 
    }

    return NO_ERROR;
} /* get_next_cmd */

int
parse_exec_args( Cmd *self,
                 String *string)
{
    int n_args = 0; // 
    
    while ( 1 )
    {            
        skip_spaces( string);

        if ( string->str[str_pos] == '|' || str_pos >= string->len )
        {
            break;
        }

        // save option
        self->exec_args[n_args] = &string->str[str_pos];
        n_args++;
        skip_exec_arg( string);

        // place '\0' to distinguish options
        string->str[str_pos] = '\0';
        str_pos++;
    }

    self->n_exec_args = n_args;
    self->exec_args[n_args] = NULL; // set NULL to end

    return n_args + 1; // skip NULL at the end of option list

} /* option_parse */

#undef str_pos


int
count_cmds( char* parse_str)
{
    if ( parse_str == NULL )
    {
        return NULLPTR;
    }
    
    int n_cmds = 1; // should be at least one command
    
    while ( *parse_str != '\0' )
    {
        if ( *parse_str == '|' )
        {
            n_cmds++;
        }
        parse_str++;
    }

    return n_cmds;
    
} /* count_cmds */

CmdArray
parse_cmds( char *parse_str)
{
    CmdArray cmd_array = { 0 };
    
    if ( parse_str == NULL )
    {
        return cmd_array;
    }
    
    int n_cmds = 0;
    int n_opts = 0;
    
    Cmd *parsed_cmds = (Cmd *) calloc( count_cmds( parse_str), sizeof( Cmd));
    const char **exec_args = (const char **) calloc( strlen( parse_str) + 1, sizeof( char*));
    
    if ( parsed_cmds == NULL || exec_args == NULL)
    {
        return cmd_array;
    }
    
    String str = {
                   .str = parse_str,
                   .pos = 0,
                   .len = strlen( parse_str)
                 };

    cmd_array.n_cmds = -1;
    cmd_array.cmds = parsed_cmds;
    cmd_array.malloc_exec_args = exec_args;

    while ( str.pos < str.len )
    {
        cmd_init( &parsed_cmds[n_cmds], exec_args + n_opts); 
        n_opts += parse_exec_args( &parsed_cmds[n_cmds], &str);

        if ( get_next_pipe( &str) == INVALID_PIPE )
        {
            printf( "Broken pipe\n");
            return cmd_array;
        }

        n_cmds++;
    }

    cmd_array.n_cmds = n_cmds;

    return cmd_array;

}


void
cmd_array_destr( CmdArray *self)
{
    free( self->cmds);
    free( self->malloc_exec_args);
    self->n_cmds = -1;
}
