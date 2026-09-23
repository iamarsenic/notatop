#include "../include/thermal.h"
#include <stdio.h>
#include <string.h>
#include <glob.h>

static glob_t g_zones = {0};
static int glob_success = 0;
static int zone_count = 0;

int init_thermal(void) {
    if (glob("/sys/class/thermal/thermal_zone*", 0, NULL, &g_zones) == 0) {
        glob_success = 1;
        zone_count = g_zones.gl_pathc;
        if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;
        return zone_count;
    }
    return -1;
}

ThermalPayload update_thermal(void) {
    ThermalPayload payload = {0};
    payload.count = zone_count;
    if (!glob_success) return payload;

    for (int i = 0; i < zone_count; i++) {
        char type_path[256];
        char temp_path[256];
        snprintf(type_path, sizeof(type_path), "%s/type", g_zones.gl_pathv[i]);
        snprintf(temp_path, sizeof(temp_path), "%s/temp", g_zones.gl_pathv[i]);

        FILE *f_type = fopen(type_path, "r");
        if (f_type) {
            if (fgets(payload.zones[i].name, sizeof(payload.zones[i].name), f_type)) {
                payload.zones[i].name[strcspn(payload.zones[i].name, "\r\n")] = '\0';
            } else {
                strcpy(payload.zones[i].name, "unknown");
            }
            fclose(f_type);
        }

        // Читаем температуру
        FILE *f_temp = fopen(temp_path, "r");
        long temp_raw = -1;
        if (f_temp) {
            if (fscanf(f_temp, "%ld", &temp_raw) != 1) temp_raw = -1;
            fclose(f_temp);
        }

        payload.zones[i].temp = (temp_raw != -1) ? (int)(temp_raw / 1000.0) : -1;
    }
    return payload;
}

void free_thermal(void) {
    if (glob_success) {
        globfree(&g_zones);
        glob_success = 0;
    }
}

