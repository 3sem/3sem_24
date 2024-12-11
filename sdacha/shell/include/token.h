#ifndef TOKEN_H_
#define TOKEN_H_

typedef enum CmdError_t
{
    INVALID_PIPE = -3,
    NULLPTR      = -2,
    MALLOC_ERROR = -1,
    NO_ERROR     = 0,
    
} CmdError_t;

typedef struct Cmd 
{
    const char **exec_args;
    int n_exec_args;
} Cmd;

typedef struct CmdArray
{
    Cmd *cmds;
    int n_cmds;
    const char **malloc_exec_args;
} CmdArray;

typedef struct String
{
    char *str;
    int pos;
    int len;
} String;

CmdError_t cmd_init( Cmd *self,
                     const char **options);
int parse_exec_args( Cmd *self,
                     String *string);
void skip_spaces( String *string);
void skip_exec_arg( String *string);
CmdError_t get_next_pipe( String *string);
int count_cmds( char *parse_str);
CmdArray parse_cmds( char *argv_str);
void cmd_array_destr( CmdArray *self);

#endif
