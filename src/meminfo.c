#include "../include/meminfo.h"
#include <stdio.h>
#include <string.h>

int parse_meminfo(SystemMemory *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    memset(mem, 0, sizeof(SystemMemory));

    char line[256];
    char label[64];
    long value;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%63s %ld", label, &value) == 2) {
            if (strcmp(label, "MemTotal:") == 0) mem->total = value;
            else if (strcmp(label, "MemFree:") == 0) mem->free = value;
            else if (strcmp(label, "MemAvailable:") == 0) mem->available = value;
            else if (strcmp(label, "Buffers:") == 0) mem->buffers = value;
            else if (strcmp(label, "Cached:") == 0) mem->cached = value;
            else if (strcmp(label, "Active:") == 0) mem->active = value;
            else if (strcmp(label, "Inactive:") == 0) mem->inactive = value;
            else if (strcmp(label, "Mapped:") == 0) mem->mapped = value;
        }
    }

    fclose(fp);
    return 0;
}
