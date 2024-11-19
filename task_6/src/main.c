#include <stdio.h>
#include "debugging.h"
#include "options.h"

int main(int argc, char const *argv[])
{
    RETURN_ON_TRUE(argc < 2, ARGC_NOT_TWO, printf("processmon: Not enough arguments. Specify process PID. For more information use -h\n"););
    RETURN_ON_TRUE(argc > 3, ARGC_TOO_MANY, printf("processmon: Too many arguments in a call. For more information use -h\n"););

    RETURN_ON_TRUE(execute_option(argv[1]) == ERR_OPT, ERR_OPT);

    return 0;
}
