#ifndef IPC_UTILS_H
#define IPC_UTILS_H

void setup_ipc(const char* fifo_path);

void cleanup_ipc(const char* fifo_path);

#endif // !IPC_UTILS_H
