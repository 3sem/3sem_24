#include <stdio.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <poll.h>
#include "inotify_func.h"
#include "debugging.h"

static inotify_descr_t inotify_fd          = 0;
//static struct inotify_event inotify_str    = {0};

int init_inotify_struct(const char *path)
{
    inotify_fd      = inotify_init();
    RETURN_ON_TRUE(inotify_fd == -1, -1, perror("inotify fd creation error"););

    int watch_decriptor = inotify_add_watch(inotify_fd, path, IN_ALL_EVENTS);
    RETURN_ON_TRUE(watch_decriptor == -1, -1, perror("> Inotify watch add error"););

    return 0;
}

void destr_inotify_struct()
{
    close(inotify_fd);

    inotify_fd      = -1;
    //inotify_str.wd  = 0;

    return;
}

unsigned int notify_about_change_in_file()
{
    struct pollfd fds   = {0};
    fds.fd              = inotify_fd;
    fds.events          = POLLIN;

    if (!poll(&fds, 1, 0))
        return 0;
    
    LOG("> inotify fd is: %d\n", fds.fd);

    struct inotify_event event_str  = {};
    ssize_t changed_stat            = read(fds.fd, &event_str, sizeof(struct inotify_event));
    RETURN_ON_TRUE(changed_stat == -1, 0, perror("Error occured while reading a file"););

    LOG("> file modification check\n");

    LOG("> event mask is: %u\n", event_str.mask);

    return event_str.mask;
}
