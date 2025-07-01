#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ssd1306.h"

static int fd = -1;

int ssd1306_init() {
    fd = open("/dev/ssd1306", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open ssd1306 dev file\n");
        return -1;
    }
    return 0;
}

void ssd1306_end() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

int ssd1306_write(const char* buf) {
    if (fd < 0) {
        return -1;
    }

    ssize_t len = strlen(buf);
    ssize_t ret = write(fd, buf, len);

    if ((ret < 0) | (ret != len)) {
        return -1;
    }

    return 0;
}