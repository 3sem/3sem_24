#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

// Проверяет, является ли файл текстовым
int is_text_file(const char *path);

// Создает или обновляет инкрементальный бэкап
void perform_backup(const char *directory);

#endif // !FILE_UTILS_H

