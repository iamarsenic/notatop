#define _GNU_SOURCE
#include "../include/thermal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>

static int g_temp_fds[MAX_ZONES] = {-1};
static char g_zone_names[MAX_ZONES][64];
static int zone_count = 0;
static int initialized = 0;

int init_thermal(void) {
    glob_t glob_result;
    if (glob("/sys/class/thermal/../hwmon/hwmon*", 0, nullptr, &glob_result) != 0) {
        return -1;
    }

    char target_path[256] = "";

    for (size_t i = 0; i < glob_result.gl_pathc; i++) {
        char name_path[256];
        char name_buf[64] = "";
        
        snprintf(name_path, sizeof(name_path), "%s/name", glob_result.gl_pathv[i]);
        int fd_name = open(name_path, O_RDONLY);
        if (fd_name >= 0) {
            ssize_t len = read(fd_name, name_buf, sizeof(name_buf) - 1);
            if (len > 0) {
                name_buf[len] = '\0';
                name_buf[strcspn(name_buf, "\r\n")] = '\0';
            }
            close(fd_name);
        }

        if (strcmp(name_buf, "coretemp") == 0 || strcmp(name_buf, "k10temp") == 0) {
            strncpy(target_path, glob_result.gl_pathv[i], sizeof(target_path) - 1);
            break; 
        }
    }

    if (strlen(target_path) == 0) {
        globfree(&glob_result);
        return -1;
    }

    glob_t temp_glob;
    char temp_pattern[256];
    snprintf(temp_pattern, sizeof(temp_pattern), "%s/temp*_input", target_path);

    if (glob(temp_pattern, 0, nullptr, &temp_glob) == 0) {
        zone_count = temp_glob.gl_pathc;
        if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;

        for (int i = 0; i < zone_count; i++) {
            g_temp_fds[i] = open(temp_glob.gl_pathv[i], O_RDONLY);

            char label_path[256];
            strncpy(label_path, temp_glob.gl_pathv[i], sizeof(label_path) - 1);
            char *sub = strstr(label_path, "_input");
            if (sub) strcpy(sub, "_label");

            int fd_label = open(label_path, O_RDONLY);
            if (fd_label >= 0) {
                ssize_t len = read(fd_label, g_zone_names[i], sizeof(g_zone_names[i]) - 1);
                if (len > 0) {
                    g_zone_names[i][len] = '\0';
                    g_zone_names[i][strcspn(g_zone_names[i], "\r\n")] = '\0';
                } else {
                    snprintf(g_zone_names[i], sizeof(g_zone_names[i]), "Core %d", i);
                }
                close(fd_label);
            } else {
                snprintf(g_zone_names[i], sizeof(g_zone_names[i]), "Core %d", i);
            }
        }
        globfree(&temp_glob);
    }

    globfree(&glob_result);
    initialized = 1;
    return zone_count;
}

ThermalPayload update_thermal(void) {
    ThermalPayload payload = {0};
    payload.count = zone_count;
    if (!initialized) return payload;

    for (int i = 0; i < zone_count; i++) {
        strncpy(payload.zones[i].name, g_zone_names[i], sizeof(payload.zones[i].name) - 1);

        long temp_raw = -1;
        if (g_temp_fds[i] >= 0) {
            char buf[32];
            ssize_t bytes_read = pread(g_temp_fds[i], buf, sizeof(buf) - 1, 0);
            if (bytes_read > 0) {
                buf[bytes_read] = '\0';
                temp_raw = strtol(buf, nullptr, 10);
            }
        }

        payload.zones[i].temp = (temp_raw != -1) ? (int)(temp_raw / 1000) : -1;
    }
    return payload;
}

void free_thermal(void) {
    if (initialized) {
        for (int i = 0; i < zone_count; i++) {
            if (g_temp_fds[i] >= 0) {
                close(g_temp_fds[i]);
                g_temp_fds[i] = -1;
            }
        }
        initialized = 0;
        zone_count = 0;
    }
}

