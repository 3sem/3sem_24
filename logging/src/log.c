#include "log.h"

static FILE *log_file = NULL;

int log_init(const char *log_name)
{
    DIR *log_dir = opendir("logs");
    if (!log_dir)
    {
        printf("creating directory\n");
        system("mkdir logs");
    }
    closedir(log_dir);
    
    char log_path[100] = {0};
    snprintf(log_path, sizeof(log_path), "logs/%s", log_name);

    log_file = fopen(log_path, "wb");
    if (!log_file)
    {
        printf("[error]>>> couldn't open logfile %s.\n", log_name);
        return FILE_OPEN_ERR;   
    }

    setbuf(log_file, NULL);

    return 0;
}

void log_msg(const int option, const char *format, ...)
{
    assert(option == MSG || option == ERR);

    char log_msg[250] = {0};

    va_list args = {};
    va_start(args, format);

    if      (option == ERR)
    {
        snprintf(log_msg, sizeof(log_msg), "[%ld]error>>> %s\n", clock(), format);
        vprintf(log_msg, args);
    }
    else
    {
        snprintf(log_msg, sizeof(log_msg), "[%ld]> %s\n", clock(), format);
    }

    if (log_file)
        vfprintf(log_file, log_msg, args);

    va_end(args);
}

void close_log()
{
    log_msg(MSG, "closing logfile");
    if (log_file)
        fclose(log_file);

    log_file = NULL;
}