#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

#define SMALL_BUFFER_SIZE  8192    // 8 KB 
#define MEDIUM_BUFFER_SIZE 65536   // 64 KB
#define LARGE_BUFFER_SIZE  1048576 // 1 MB

typedef struct
{
    char* buffer;
    size_t capacity;
    size_t size;

} Buffer;

Buffer* CreateBuffer (size_t capacity);

int DestroyBuffer (Buffer* buffer);

int writeToFile (Buffer* buffer, int fileDesc);

int readFromFile (Buffer* buffer, int fileDesc);

int CreateFile(const char* filename);

int OpenFile(const char* filename);

#endif // BUFFER_H
