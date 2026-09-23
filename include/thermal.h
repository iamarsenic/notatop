#pragma once

#define MAX_ZONES 10

typedef struct {
    char name[64];
    int temp;
} ZonePayload;

typedef struct {
    ZonePayload zones[MAX_ZONES];
    int count;
} ThermalPayload;

int init_thermal(void);
ThermalPayload update_thermal(void);
void free_thermal(void);
