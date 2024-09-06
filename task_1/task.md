### Задача 1
Разработать эмулятор терминала, позволяющий:
- Запускать программы, циклически считывая команды через STDIN и запуская их в процессах-потомках.
- Анализировать коды возврата завершенных процессов
- Запускать конвейер процессов, например «env| grep HOSTNAME | wc». Постарайтесь при этом минимизировать количество открытых одновременно файловых дескрипторов.

## English Version

### Homework 1: 
  Write a 'shell-wrapper' program which allow users to:
- run programs via command line cyclically getting commands from STDIN and running it somewhere, e.g. in child process.
- get exit codes of terminated programs
- use pipelining of program sequences, for ex.: «env | grep HOSTNAME | wc»
### TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (for ex, see "man execlp").
   - if execvp is used, string splitting into "path" and "args" is all you need.
   - the program should be environment($PATH variable)-sensitive: for ex.,'/bin/ls' should be executed with string 'ls'. 
3. Collect exit codes via waitpid/WEXITSTATUS.
4. Note that a size of command can reach a lot of kbytes: type "getconf ARG_MAX" cmd to check it.
5. Let the cmd can represent a sequence of programs which can be executed sequentially, if ‘|’ symbol is between programs’ names, 
in this case output of i-1’th should be redirected from stdout to stdin of i'th. See the example #10 (will be added after the seminar #2). Implement this feature.
6. Try to minimize the number of file descriptors opened at the same time.
