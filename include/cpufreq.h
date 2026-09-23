#pragma once

#define MAX_CORES 32

typedef struct {
    int count;
    long freqs[MAX_CORES];
} CPUFreqPayload;

int init_cpufreq(void);
CPUFreqPayload update_cpufreq(void);
void free_cpufreq(void);
