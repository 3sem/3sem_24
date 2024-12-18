#ifndef DEAMON_PROC
#define DEAMON_PROC

#define DEAMON_PID_DIR  "/tmp/processmon_pid.txt"

int run_deamon(const int pid_to_monitor);

#endif