#pragma once

typedef struct {
	long mem_total;
	long mem_available;
	long swap_total;
	long swap_free;
} SystemMemory;

int parse_meminfo(SystemMemory *mem);
