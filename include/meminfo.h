#pragma once

typedef struct {
	long mem_total;
	long mem_available;
	long mapped;
} SystemMemory;

int parse_meminfo(SystemMemory *mem);
