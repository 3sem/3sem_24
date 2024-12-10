#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <string.h>

int monitored_pid = 0;
int sampling_interval = 1000;

// Default configuration values
#define DEFAULT_PID 1
#define DEFAULT_INTERVAL 1000

// Function to create a default configuration file
void initialize_default_config() {
    FILE *config_file = fopen(CONFIG_FILE, "w");
    if (!config_file) {
        syslog(LOG_ERR, "Failed to create default config file: %s", CONFIG_FILE);
        return;
    }

    fprintf(config_file, "pid=%d\n", DEFAULT_PID);
    fprintf(config_file, "interval=%d\n", DEFAULT_INTERVAL);
    fclose(config_file);

    syslog(LOG_INFO, "Default configuration created: %s", CONFIG_FILE);
}

// Load the configuration file
void load_config() {
    FILE *config_file = fopen(CONFIG_FILE, "r");

    if (!config_file) {
        syslog(LOG_WARNING, "Configuration file not found: %s", CONFIG_FILE);
        syslog(LOG_WARNING, "Initializing default configuration.");
        initialize_default_config();
        monitored_pid = DEFAULT_PID;
        sampling_interval = DEFAULT_INTERVAL;
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        if (sscanf(line, "pid=%d", &monitored_pid) == 1) {
            syslog(LOG_INFO, "Monitored pid set to %d", monitored_pid);
        } else if (sscanf(line, "interval=%d", &sampling_interval) == 1) {
            syslog(LOG_INFO, "Sampling interval set to %d", sampling_interval);
        } else {
            syslog(LOG_ERR, "Invalid line in config file: %s", line);
        }
    }

    fclose(config_file);
}

