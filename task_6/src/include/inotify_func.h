#ifndef INOTIFY_FUNCTIONS
#define INOTIFY_FUNCTIONS

typedef int inotify_descr_t;


int init_inotify_struct(const char *path);

void destr_inotify_struct();

unsigned int notify_about_change_in_file();

#endif