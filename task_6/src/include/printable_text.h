#ifndef LINE_CONSTANTS
#define LINE_CONSTANTS

#define HELP_MESSAGE \
"Processmon is a service to actively monitor process's changes in memory mapping\n\
Usage: processmon [option] PID_number\n\
Given options are:\n\
[-d]: run programm in the background\n\
[-i]: run programm in interactive mode\n\
[-h]: show this menu\n\
Note: there can be only 1 option. If no option is given, process runs in interactive mode\n\
Interacting in the background:\n\
\"chpid PID_number\" - change id of a process that is being monitored\n\
\"chprd TIME\" - change the period of how frequently monitoring will happen\n"

#define INTERACTIVE_MESSAGE \
"processmon: your programm is running in interactive mode\n\
[1] - change proccess for monitoring\n\
[2] - change the frequency of how often monitoring will happen\n\
[3] - change the temporary folder path\n\
[4] - end the programm\n\
[5] - save current parameters to standard config\n\
[6] - show current standard config\n\
[7] - switch between saving or deleting temporary directory\n\
> "

#define MEM_ALC_ERR_MSG     "Processmon: buffer memory allocation error\n"

#define INPUT_ERR_MSG       "Input error, please enter a number beetwen 1 and 5\n"

#define SAVING_CONFIG_MSG   "Processmon: saving config. Wait for 1 monitor period in order for config to apply\n"

#define CUR_PAR_MSG         "Processmon: current parameters are:\n"

#define CUR_PID             "CURRENT PID: %d\n"

#define LINE_MSG            "\n----------------------------------------------------------------\n"

#define CHANGING_PID_MSG    "Changing the PID. All diff files will be deleted for the new process. Please enter new process ID:\n> "

#define NEW_PID_MSG         "New PID is: %d\n"

#define CHANGE_PRD_MSG      "Changing monitoring period. Please enter new time in seconds:\n>"

#define NEW_PERIOD_MSG      "New period is: %u. Wait for the period to apply\n"

#define NEW_PERIOD_ERR      "Processmon: period you entered can't be set, please enter a positive number\n"

#define CHANGE_TMP_DIR      "Changing diff file location. Please enter new path:\n>"

#define NEW_DIR_MSG         "New directory path is: \"%s\"\n"

#define FINISHING_MSG       "Finishing programm.\n"

#define NUM_INPUT_ERR       "Input error, please enter a number\n"

#define NOT_ENOUGH_ARGS_ERR "Processmon: Not enough arguments. Specify process PID. For more information use -h\n"

#define TOO_MANY_ARGS_ERR   "Processmon: Too many arguments in a call. For more information use -h\n"

#define INVALID_ARG_ERR     "Processmon: invalid argument. Use -h for info\n"

#define CFG_CREATE_MSG      "Processmon: creating standard config file\n"

#define CFG_PARSE_ERR_MSG   "Processmon: couldn't parse config properly, attempting to create new config\n"

#define DIR_OPEN_ERR_MSG    "Processmon: couldn't open temporary directory for changing\n"

#define PATH_NOT_DIR_MSG    "Processmon: path you entered is not a directory\n"

#define FILE_DELETED_MSG    "Processmon: it looks like the process you watching has been terminated, finishing programm\n"

#define MAP_OPEN_ERR_MSG    "Processmon: couldn't open map file for reading\n"

#endif