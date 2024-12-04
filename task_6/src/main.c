#include <stdio.h>
#include "debugging.h"
#include "options.h"

//TO DO: сделать стандартный конфиг файл, сделать демона, добавить новые команды управления программой, перенести большую часть строковых констант в отдельный хедер
//TO FIX: починить перемещение временной директории

int main(int argc, char const *argv[])
{
    RETURN_ON_TRUE(argc < 2, ARGC_NOT_TWO, printf("processmon: Not enough arguments. Specify process PID. For more information use -h\n"););
    RETURN_ON_TRUE(argc > 3, ARGC_TOO_MANY, printf("processmon: Too many arguments in a call. For more information use -h\n"););

    RETURN_ON_TRUE(execute_option(argv) == ERR_OPT, ERR_OPT);

    return 0;
}
