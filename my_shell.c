#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 1000  // максимальная длина строки
#define MAX_ARGS 1000   // максимальное количество аргументов

void run_command(char *command) 
{
    char *args[MAX_ARGS];
    char *p;
    int i = 0;

    // Разделяем команду на аргументы
    char delim[] = " \n";
    p = strtok(command, delim);
    while (p != NULL && i < MAX_ARGS) 
    {
        args[i++] = p;
        p = strtok(NULL, delim);
    }
    args[i] = NULL;  // Завершаем массив NULL
    if (fork() == 0) 
    {  // Дочерний процесс
        execvp(args[0], args);  // Выполняем команду
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } 
    else 
    {  // Родительский процесс
        int status;
        wait(&status);  // Ждем завершения дочернего процесса
        if (WIFEXITED(status)) 
        {
            printf("ret code: %d\n", WEXITSTATUS(status));
    	}
    }	
}
void run_pipeline(char *pipeline) 
{
    char *commands[MAX_ARGS];
    char *p;
    int num_commands = 0;
    char delim[] = "|";
    // Разделяем конвейер на команды
    p = strtok(pipeline, delim);
    while (p != NULL && num_commands < MAX_ARGS) 
    {
        commands[num_commands++] = p;
        p = strtok(NULL, delim);
    }
    commands[num_commands] = NULL;  // Завершаем массив NULL

    int fd[2];
    int input_fd = 0;  // Входной файл-дескриптор для первой команды

    for (int i = 0; i < num_commands; i++) 
    {
        pipe(fd);  // Создаем конвейер
        
        if (fork() == 0) 
        {  // Дочерний процесс
            dup2(input_fd, 0);  // Перенаправляем входные данные
            if (i < num_commands - 1) 
            {
                dup2(fd[1], 1);  // Перенаправляем выходные данные
            }
            close(fd[0]);  // Закрываем неиспользуемый конец
            run_command(commands[i]);
            exit(0);
        } 
        else 
        {
            wait(NULL);  // Ждем завершения дочернего процесса
            close(fd[1]);  // Закрываем конец конвейера
            input_fd = fd[0];  // Устанавливаем входной файл-дескриптор для следующей команды
        }
    }
}

int main() 
{
    char line[MAX_LENGTH];

    while (1) 
    {
        printf("> ");  // Выводим приглашение
        if (!fgets(line, sizeof(line), stdin)) 
        {
            break;  // Выход при ошибке ввода
        }

        if (strchr(line, '|')) 
        {
            run_pipeline(line);  // Запуск конвейера
        } 
        else 
        {
            run_command(line);  // Запуск обычной команды
        }
    }

    return 0;
}
