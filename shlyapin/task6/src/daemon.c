#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

#include "daemon.h"
#include "config.h"
#include "utils.h"
#include "monitoring.h"

typedef struct {
    char buf[DAEMON_HEADER_MAX];
    int length;
    char *name;
    int pid;
} DaemonHeader;

static const int DATE_LENGTH = 256;

static void create_daemon_header(DaemonHeader *header);
static void write_deamon_header();

void daemon_vasya(int monitoring_pid) {
    // Создаём дочерний процесс
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Родитель завершает работу
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    printf("Daemon PID: %d\n\n", getpid());

    // Дочерний процесс становится лидером сессии
    if (setsid() < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    // Закрываем и переопределяем файловые дескрипторы
    close(STDIN_FILENO);

    int fd_out = open(DAEMON_LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_out < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    int fd_err = open(DAEMON_ERROR_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_err < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    dup2(fd_err, STDERR_FILENO);
    
    // Меняем рабочий каталог
    if (chdir(DAEMON_DIR) < 0) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    write_deamon_header();

    start_monitoring(monitoring_pid);

    close(fd_out);
    close(fd_err);
}

static void write_deamon_header() {
    DaemonHeader header = {.length = 0, .name = DAEMON_NAME, .pid = getpid()};
    create_daemon_header(&header);

    printf("%s", header.buf);
    fprintf(stderr, "%s", header.buf);

    FILE *f = fopen(DAEMON_PID_FILE, "w");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%d", header.pid);
    fclose(f);
}

// Функция для создания заголовка демона
static void create_daemon_header(DaemonHeader *header) {
    // Получаем текущую дату и время
    char datetime[TIME_MAX];
    get_current_time(datetime, TIME_MAX);

    // Получаем рабочую директорию демона
    char work_dir[PATH_MAX];
    get_work_dir(work_dir, header->pid);

    // Получаем путь к директории процесса
    char proc_dir[PATH_MAX];
    get_proc_dir(proc_dir, header->pid);

    // Формируем табличку
    header->length = sprintf(header->buf,
                        "\n\n\nMain daemon information\n"
                        "\tDaemon create time: %s\n"
                        "\tDaemon name: %s\n"
                        "\tDaemon PID: %d\n"
                        "\tDaemon work directory: %s\n"
                        "\tDaemon proc directory: %s\n\n\n",
                        datetime, header->name, header->pid, work_dir, proc_dir
    );
}
