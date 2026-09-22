#pragma once

typedef struct {
	long total;
	long free;
	long available;
	long mapped;
	long cached;
	long active;
	long inactive;
} SystemMemory;

int parse_meminfo(SystemMemory *mem);
