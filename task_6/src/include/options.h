#ifndef OPTIONS
#define OPTIONS

unsigned int    parse_options   (const char *option);

int             execute_option  (char const *argv[]);

enum OPTS
{
    DEAMON          =   1,
    INTERACTIVE     =   2,
    HELP            =   3,
    INTERACTIVE_IMP =   4,
    ERR_OPT         =   5
};

#endif