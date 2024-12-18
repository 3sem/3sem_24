#include <stdio.h>
#include "debugging.h"
#include "options.h"

//TO DO: cделать демона, сделать обработку дифф файлов

int main(int argc, char const *argv[])
{
    RETURN_ON_TRUE(argc < 2, ARGC_NOT_TWO, printf(NOT_ENOUGH_ARGS_ERR););
    RETURN_ON_TRUE(argc > 3, ARGC_TOO_MANY, printf(TOO_MANY_ARGS_ERR););

    RETURN_ON_TRUE(execute_option(argv) == ERR_OPT, ERR_OPT);

    return 0;
}
