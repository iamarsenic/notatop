#define _GNU_SOURCE
#include "../include/meminfo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static inline long parse_field(const char *buf, const char *label) {
    const char *pos = strstr(buf, label);
    if (pos) {
        return strtol(pos + strlen(label), nullptr, 10);
    }
    return 0;
}

int parse_meminfo(SystemMemory *mem) {
    static int mem_fd = -1;
    static bool init_failed = false;

    if (init_failed) return -1;

    if (mem_fd < 0) {
        mem_fd = open("/proc/meminfo", O_RDONLY);
        if (mem_fd < 0) {
            init_failed = true;
            return -1;
        }
    }

    if (lseek(mem_fd, 0, SEEK_SET) == -1) {
        return -1;
    }

    char buffer[2048];
    ssize_t bytes_read = read(mem_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return -1;
    }
    buffer[bytes_read] = '\0'; // close this

    memset(mem, 0, sizeof(SystemMemory));

    mem->total     = parse_field(buffer, "MemTotal:");
    mem->free      = parse_field(buffer, "MemFree:");
    mem->available = parse_field(buffer, "MemAvailable:");
    mem->buffers   = parse_field(buffer, "Buffers:");
    mem->cached    = parse_field(buffer, "Cached:");
    mem->active    = parse_field(buffer, "Active:");
    mem->inactive  = parse_field(buffer, "Inactive:");
    mem->mapped    = parse_field(buffer, "Mapped:");

    return 0;
}

