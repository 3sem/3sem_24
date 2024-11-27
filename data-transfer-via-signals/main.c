#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void start_receiver(const char *output_file, pid_t *receiver_pid) {
    *receiver_pid = fork();
    
    if (*receiver_pid == -1) 
    {
        perror("Fork failed");
        exit(1);
    }

    if (*receiver_pid == 0) 
    {
        // Child process: run the receiver
        printf("Starting receiver...\n");
        char *args[] = {"./receiver", (char *)output_file, NULL};
        execvp(args[0], args);
        // If execvp fails
        perror("Receiver exec failed");
        exit(1);
    }
}

void start_sender(pid_t receiver_pid, const char *input_file) {
    printf("Starting sender with receiver PID: %d...\n", receiver_pid);
    char receiver_pid_str[10];
    snprintf(receiver_pid_str, sizeof(receiver_pid_str), "%d", receiver_pid);

    char *args[] = {"./sender", receiver_pid_str, (char *)input_file, NULL};
    execvp(args[0], args);

    perror("Sender exec failed");
    exit(1);
}

int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        printf("Usage: %s <output_file> <file_to_send>\n", argv[0]);
        return 1;
    }

    const char *output_file = argv[1];
    const char *input_file = argv[2];

    pid_t receiver_pid;

    start_receiver(output_file, &receiver_pid);

    sleep(1);

    start_sender(receiver_pid, input_file);

    waitpid(receiver_pid, NULL, 0);

    printf("File transfer complete.\n");
    return 0;
}

