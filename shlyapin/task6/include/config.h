#pragma once

#include <limits.h>

#define DAEMON_NAME       "Daemon Vasya"

#define DAEMON_DIR        "/daemons/vasya"
#define DAEMON_LOG_FILE   "/daemons/vasya/vasya.log"
#define DAEMON_ERROR_FILE "/daemons/vasya/vasya.err"
#define DAEMON_PID_FILE   "/daemons/vasya/vasya.pid"

#define DAEMON_HEADER_MAX  (4 * PATH_MAX)

#define INOTIFY_WATCHES_MAX 1024
