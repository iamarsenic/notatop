#pragma once

typedef struct {
    long total;
    long free;
    long available;
    long buffers;
    long cached;
    long active;
    long inactive;
    long mapped;
} SystemMemory;

int parse_meminfo(SystemMemory *mem);
