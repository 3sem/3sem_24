#define _DEFAULT_SOURCE
#include "file_utils.h"
#include <linux/limits.h>
#include <sys/stat.h>
#include <time.h>

#define BACKUP_DIR "/home/aidenfmunro/Programming/Linux-Course/daemon/backup_daemon" // Путь для хранения бэкапов

int is_text_file(const char *path) 
{
    char command[256];
    snprintf(command, sizeof(command), "file --mime %s | grep -q text", path);

    return (system(command) == 0);
}

// Создание каталога для бэкапов
static void ensure_backup_dir(const char *path) 
{
    struct stat st;

    if (stat(path, &st) == -1) 
    {
        if (mkdir(path, 0755) == -1) 
        {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }
}

// Генерация имени файла для бэкапа
static void generate_backup_name(const char *file_path, char *backup_name, size_t size) 
{
    time_t now = time(NULL);
    snprintf(backup_name, size, "%s/%ld.diff", BACKUP_DIR, now);
}

// Выполнение инкрементального бэкапа
void perform_backup(const char *directory) 
{
    ensure_backup_dir(BACKUP_DIR);

    DIR *dir = opendir(directory);

    if (!dir) 
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type != DT_REG) continue;

        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (is_text_file(file_path)) 
        {
            printf("Processing text file: %s\n", file_path);

            char backup_file[PATH_MAX];
            snprintf(backup_file, sizeof(backup_file), "%s/%s", BACKUP_DIR, entry->d_name);

            if (access(backup_file, F_OK) != 0) 
            {
                // Первый бэкап: копируем файл
                printf("Creating full backup for %s\n", file_path);
                char command[PATH_MAX * 2];
                snprintf(command, sizeof(command), "cp %s %s", file_path, backup_file);
                system(command);
            } 
            else 
            {
                // Инкрементальный бэкап: создаем diff
                char diff_file[PATH_MAX];
                generate_backup_name(file_path, diff_file, sizeof(diff_file));
                char command[PATH_MAX * 3];
                snprintf(command, sizeof(command), "diff -u %s %s > %s", backup_file, file_path, diff_file);
                system(command);

                // Применяем изменения для обновления "актуального" состояния
                snprintf(command, sizeof(command), "patch %s < %s", backup_file, diff_file);
                system(command);
            }
        }
    }

    closedir(dir);
}

