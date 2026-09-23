#define _GNU_SOURCE
#include "../include/fan.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>

long get_fan_rpm(void) {
    static int fan_fd = -1;
    static bool initialized = false;
    static bool init_failed = false;

    if (init_failed) return -1;

    if (!initialized) {
        glob_t glob_result;
        if (glob("/sys/devices/platform/thinkpad_hwmon/hwmon/hwmon*/fan1_input", 0, nullptr, &glob_result) == 0) {
            fan_fd = open(glob_result.gl_pathv[0], O_RDONLY);
            initialized = true;
        }
        globfree(&glob_result);

        if (fan_fd < 0) {
            init_failed = true;
            return -1;
        }
    }

    char buf[32];
    ssize_t bytes_read = pread(fan_fd, buf, sizeof(buf) - 1, 0);
    if (bytes_read <= 0) {
        return -1;
    }
    
    buf[bytes_read] = '\0';

    return strtol(buf, nullptr, 10);
}

