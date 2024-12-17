#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "config.h"

static const int COMMAND_MAX     = 2 * PATH_MAX;
static const int COMMAND_RES_MAX = 1024;

int is_type_file(const char *file_path, const char *ftype) {
    char command[COMMAND_MAX];
    char result[COMMAND_RES_MAX];

    // Формируем команду для определения MIME-типа файла
    snprintf(command, COMMAND_MAX, "file --brief --mime-type %s", file_path);
    
    // Выполняем команду и считываем результат
    FILE *fpath = popen(command, "r");
    if (fpath == NULL) {
        perror("Error executing popen instruction");
        return -1; // Ошибка
    }

    if (fgets(result, COMMAND_RES_MAX, fpath) == NULL) {
        perror("Error reading file command result");
        pclose(fpath);
        return -1; // Ошибка
    }

    pclose(fpath);

    // Сравниваем результат с MIME-типом
    return (strncmp(result, ftype, strlen(ftype)) == 0 && result[strlen(ftype)] == '/');
}

// Функция для получения текущей даты и времени в формате строки
void get_current_time(char *buffer, size_t size) {
    time_t rawtime = 0;
    time(&rawtime);

    struct tm *timeinfo = localtime(&rawtime);

    strftime(buffer, size, "%d.%m.%Y_%H:%M:%S", timeinfo);
}

//Функция для вычисления рабочей директории процесса по его PID
void get_work_dir(char *res_path, pid_t pid) {
    char path[PATH_MAX];

    // Строим путь до файла cwd в /proc/<pid>/cwd
    snprintf(path, sizeof(path), "/proc/%d/cwd", pid);

    // Читаем символическую ссылку на рабочую директорию
    ssize_t len = readlink(path, path, sizeof(path) - 1);
    if (len == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    // Завершаем строку null-терминатором
    path[len] = '\0';

    strcpy(res_path, path);
}

// Функция для вычисления пути к директории процесса по его PID
void get_proc_dir(char *res_path, pid_t pid) {
    snprintf(res_path, PATH_MAX, "/proc/%d", pid);
}

void get_full_path(char *full_path, const char *dir_path, const char *name) {
    if (strlen(dir_path) == 1 && dir_path[0] == '/') {
        snprintf(full_path, PATH_MAX, "/%s", name);
    } else {
        snprintf(full_path, PATH_MAX, "%s/%s", dir_path, name);
    }
}

int is_backup(const char *path) {
    char tmp_path[PATH_MAX];
    char tmp_path2[PATH_MAX];
    strcpy(tmp_path, path);
    change_slash_to_underscore(tmp_path);
    snprintf(tmp_path2, PATH_MAX, "%s/%s.bak", DAEMON_DIR, tmp_path);

    return access(tmp_path2, F_OK) == 0;
}

// Функция меняет все символы '/' на '_'
void change_slash_to_underscore(char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        if (str[i] == '/') {
            str[i] = '%';
        }
    }
}