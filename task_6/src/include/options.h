#ifndef OPTIONS
#define OPTIONS

unsigned int    parse_options   (const char *option);

int             execute_option  (const char *option_line);

enum OPTS
{
    DEAMON       =   1,
    INTERACTIVE  =   2,
    HELP         =   3,
    ERR_OPT      =   5
};

#endif