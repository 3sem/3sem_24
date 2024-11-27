#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv);
        return 1;
    }

    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return 1;
    }

    int wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_DELETE | IN_MODIFY);
    if (wd == -1) {
        perror("inotify_add_watch");
        return 1;
    }

    char buffer[BUF_LEN];
    while (1) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            return 1;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    printf("File %s created.\n", event->name);
                } else if (event->mask & IN_DELETE) {
                    printf("File %s deleted.\n", event->name);
                } else if (event->mask & IN_MODIFY) {
                    printf("File %s modified.\n", event->name);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    (void)inotify_rm_watch(fd, wd);
    (void)close(fd);
    return 0;
}
