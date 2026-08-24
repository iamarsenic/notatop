#include <stdio.h>
#include <glob.h>

#include "../include/read.h"

int read_string(const char *pattern, char buffer[][64], size_t max_items, size_t buffer_size) {
	int i = 0;
	glob_t glob_result;
	if (glob(pattern, 0, NULL, &glob_result) != 0) {
		return 0;
	}

	size_t count = glob_result.gl_pathc;
	if (count > max_items) {
		count = max_items;
	}

	for (size_t i = 0; i < count; i++) {
		buffer[i][0] = '\0';

		FILE *f = fopen(glob_result.gl_pathv[i], "r");
		if (!f) {
			snprintf(buffer[i], buffer_size, "unknown");
			continue;
		}

		if (fgets(buffer[i], buffer_size, f) != NULL) {
			for (size_t j = 0; j < buffer_size && buffer[i][j] != '\0'; j++) {
				if (buffer[i][j] == '\n' || buffer[i][j] == '\r') {
					buffer[i][j] = '\0';
					break;
				}
			}
		} else {
			snprintf(buffer[i], buffer_size, "unknown");
		}
		fclose(f);
	}    
	globfree(&glob_result);
	return (int)count;
}

long read_long(const char *path) {
	FILE *fl = fopen(path, "r");
	if (!fl) return -1;

	long val = -1;
	if (fscanf(fl, "%ld", &val) != 1) {
		val = -1;
	}

	fclose(fl);
	return val;
}
