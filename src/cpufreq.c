#define _GNU_SOURCE
#include "../include/cpufreq.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>

static int g_freq_fds[MAX_CORES] = {-1};
static int core_count = 0;
static bool initialized = false;

int init_cpufreq(void) {
    glob_t glob_result;
    
    if (glob("/sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq", 0, nullptr, &glob_result) == 0) {
        core_count = glob_result.gl_pathc;
        if (core_count > MAX_CORES) core_count = MAX_CORES;

        for (int i = 0; i < core_count; i++) {
            g_freq_fds[i] = open(glob_result.gl_pathv[i], O_RDONLY);
        }
        globfree(&glob_result);
        initialized = true;
        return core_count;
    }
    
    return -1;
}

CPUFreqPayload update_cpufreq(void) {
    CPUFreqPayload payload = { .count = core_count };
    if (!initialized) return payload;

    for (int i = 0; i < core_count; i++) {
        if (g_freq_fds[i] >= 0) {
            char buf[32];
            ssize_t bytes_read = pread(g_freq_fds[i], buf, sizeof(buf) - 1, 0);
            if (bytes_read > 0) {
                buf[bytes_read] = '\0';
                long khz = strtol(buf, nullptr, 10);
                payload.freqs[i] = khz / 1000;
            } else {
                payload.freqs[i] = -1;
            }
        }
    }
    return payload;
}

void free_cpufreq(void) {
    if (initialized) {
        for (int i = 0; i < core_count; i++) {
            if (g_freq_fds[i] >= 0) {
                close(g_freq_fds[i]);
                g_freq_fds[i] = -1;
            }
        }
        initialized = false;
        core_count = 0;
    }
}

