#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/meminfo.h"

int parse_meminfo(SystemMemory *mem) {
	FILE *file = fopen("/proc/meminfo", "r");
	if (file == NULL) {
		perror("Error opening /proc/meminfo");
		return -1;
	}

	char buffer[256];

	mem->mem_total		= 0;
	mem->mem_available	= 0;
	mem->swap_total		= 0;
	mem->swap_free		= 0;

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		buffer[strcspn(buffer, "\r\n")] = 0;

		char *key = strtok(buffer, ": \t");
		if (key == NULL) continue;

		char *value_str = strtok(NULL, ": \t");
		if (value_str == NULL) continue;

		long val = atol(value_str);

		if (strcmp(key, "MemTotal") == 0) {
			mem->mem_total = val;
		} else if (strcmp(key, "MemAvailable") == 0) {
			mem->mem_available = val;
		} else if (strcmp(key, "SwapTotal") == 0) {
			mem->swap_total = val;
		} else if (strcmp(key, "SwapFree") == 0) {
			mem->swap_free = val;
		}
	}

	fclose(file);
	return 0;
}
