#ifndef IO_METHODS_H
#define IO_METHODS_H

typedef struct _File {
    size_t size;
    int    file_d;
} File;

File create_file(const char* filename);
int delete_file(File* file);

size_t send_data_to_child(Pipe* self);
size_t send_data_to_parent(Pipe* self);
size_t receive_data_from_parent(Pipe* self);
size_t receive_data_from_child(Pipe* self);

#endif