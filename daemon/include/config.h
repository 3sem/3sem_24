#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE "/home/aidenfmunro/Programming/Linux-Course/daemon/backup_daemon.conf"

extern int monitored_pid;
extern int sampling_interval;

void load_config(void);

void initialize_default_config();

#endif // !CONFIG_H
