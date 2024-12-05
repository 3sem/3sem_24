#ifndef IO_METHODS_H
#define IO_METHODS_H

typedef struct _File {
    size_t size;
    int    file_d;
} File;

File create_file(const char* filename);
int delete_file(File* file);

size_t send_data_to_child(Pipe* self, File* file);
size_t send_data_to_parent(Pipe* self, File* file);
size_t receive_data_from_parent(Pipe* self, File* receive_file);
size_t receive_data_from_child(Pipe* self, File* receive_file);

#endif