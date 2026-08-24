#pragma once

#include <stddef.h>

int read_string(const char *pattern, char buffer[][64], size_t max_items, size_t buffer_size);
long read_long(const char *path);
