#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <sys/stat.h>

#include "config.h"
#include "monitoring.h"
#include "utils.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUFFER_SIZE (1024 * (EVENT_SIZE + NAME_MAX + 1))
#define DATE_LENGTH 20
#define COMAND_SIZE 1024

#define PRINT_LOG(str) printf(str);

static const int COMMAND_MAX = 3 * PATH_MAX;

typedef struct {
    int wd;
    char path[PATH_MAX];
} InotifyWatch;

typedef struct {
    InotifyWatch watches[INOTIFY_WATCHES_MAX];
    int watches_count;
    int inotify_fd;
} InotifyData;

static void add_watch_recursive(InotifyData *data, const char *path);
static void create_backup(const char *path);
static void create_patch(const char *path);
static char *get_watch_path(InotifyData *data, int wd);
static void set_watch_path(InotifyData *data, int wd, const char *path);
static void process_events(InotifyData *data);

void start_monitoring(int monitoring_pid) {
    // Создаём inotify instance
    InotifyData data = {.watches_count = 0, .inotify_fd = -1};
    data.inotify_fd = inotify_init();
    if (data.inotify_fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Получаем рабочую директорию процесса
    char work_dir[PATH_MAX];
    get_work_dir(work_dir, monitoring_pid);
    printf("Рабочая директория: %s\n", work_dir);

    add_watch_recursive(&data, work_dir);

    // Обрабатываем события
    process_events(&data);

    // Закрываем inotify
    close(data.inotify_fd);
}

// Добавление наблюдения за директорией и её подкаталогами
static void add_watch_recursive(InotifyData *data, const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Добавляем наблюдение за текущей директорией
    int wd = inotify_add_watch(data->inotify_fd, path, IN_CREATE | IN_MODIFY | IN_DELETE | IN_ISDIR);
    if (wd < 0) {
        perror("inotify_add_watch");
    } else {
        printf("Добавлено наблюдение за: %s\n", path);
    }
    set_watch_path(data, wd, path);

    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем "." и ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Формируем полный путь
        char subpath[PATH_MAX];
        if (strlen(path) <= 0) {
            perror("Path is empty");
            exit(EXIT_FAILURE);
        } else {
            get_full_path(subpath, path, entry->d_name);
        }

        // Создание бекапа файла в директории демона с расширением .bak
        if (entry->d_type == DT_REG) {
            create_backup(subpath);
        }

        // Если это директория, добавляем её в наблюдение
        if (entry->d_type == DT_DIR && strcmp(subpath, DAEMON_DIR) != 0) {
            add_watch_recursive(data, subpath);
        }
    }

    closedir(dir);
}

// Основной обработчик событий
static void process_events(InotifyData *data) {
    char buffer[EVENT_BUFFER_SIZE];

    while (1) {
        sleep(1);
        int length = read(data->inotify_fd, buffer, EVENT_BUFFER_SIZE);
        if (length < 0) {
            perror("read");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->len) {
                printf("Событие: ");
                
                // Получаем полный путь до файла
                char subpath[PATH_MAX];
                get_full_path(subpath, get_watch_path(data, event->wd), event->name);

                if (event->mask & IN_CREATE) {
                    printf("Создан: %s\n", event->name);

                    if (event->mask & IN_ISDIR) {
                        add_watch_recursive(data, subpath);
                    } else if (is_type_file(subpath, "text")) {
                        create_backup(subpath);
                    }
                } else if (event->mask & IN_MODIFY) {
                    printf("Изменён: %s\n", event->name);
                    
                    if (is_type_file(subpath, "text")) {
                        if (is_backup(subpath)) {
                            create_patch(subpath);
                        } else {
                            create_backup(subpath);
                        }
                    }
                } else if (event->mask & IN_DELETE) {
                    printf("Удалён: %s\n", event->name);
                }
            }

            i += EVENT_SIZE + event->len;
        }
    }
}


static void create_backup(const char *path) {
    char tmp_path[PATH_MAX];
    strcpy(tmp_path, path);
    
    // Формируем путь до бекапа
    change_slash_to_underscore(tmp_path);
    char backup_path[PATH_MAX];
    snprintf(backup_path, PATH_MAX, "%s/%s.bak", DAEMON_DIR, tmp_path);

    // Формируем команду для копирования файла
    char command[COMMAND_MAX];
    snprintf(command, COMMAND_MAX, "cp %s %s", path, backup_path);

    // Выполняем команду
    int ret = system(command);
    if (ret > 1) {
        fprintf(stderr, "Ошибка при создании бекапа файла\n");
    } else {
        printf("Создан бекап файла: %s\n", backup_path);
    }
}

// Функция для создания патча с помощью diff
void create_patch(const char *path) {
    char datatime[TIME_MAX];
    get_current_time(datatime, TIME_MAX);

    char tmp_path[PATH_MAX];
    strcpy(tmp_path, path);
    change_slash_to_underscore(tmp_path);

    char patch_filename[PATH_MAX];
    snprintf(patch_filename, PATH_MAX, "%s/%s_%s.patch", DAEMON_DIR, tmp_path, datatime);

    char backup_path[PATH_MAX];
    snprintf(backup_path, PATH_MAX, "%s/%s.bak", DAEMON_DIR, tmp_path);

    // Формируем команду для создания патча
    char command[COMAND_SIZE];
    snprintf(command, sizeof(command), "diff -u %s %s > %s", path, backup_path, patch_filename);

    // Выполняем команду
    int ret = system(command);
    if (ret > 1) {
        fprintf(stderr, "Ошибка при создании патча\n");
    } else {
        printf("Патч сохранён в файл: %s\n", patch_filename);
    }
}


char *get_watch_path(InotifyData *data, int wd) {
    for (int i = 0; i < data->watches_count; ++i) {
        if (data->watches[i].wd == wd) {
            return data->watches[i].path;
        }
    }
    return NULL;
}

void set_watch_path(InotifyData *data, int wd, const char *path) {
    if (strlen(path) >= PATH_MAX) {
        fprintf(stderr, "Path is too long\n");
        return;
    }
    if (data->watches_count >= INOTIFY_WATCHES_MAX) {
        fprintf(stderr, "Too many watches\n");
        return;
    }

    data->watches[data->watches_count].wd = wd;
    strcpy(data->watches[data->watches_count].path, path);
    data->watches_count++;
}