#include "../include/thermal.h"
#include <stdio.h>
#include <string.h>
#include <glob.h>

// Будем удерживать файлы температур открытыми
static FILE *g_temp_files[MAX_ZONES] = {nullptr};
static char g_zone_names[MAX_ZONES][64];
static int zone_count = 0;
static int initialized = 0;

int init_thermal(void) {
    glob_t glob_result;
    
    // Ищем все папки hwmon в системе
    if (glob("/sys/class/thermal/../hwmon/hwmon*", 0, nullptr, &glob_result) != 0) {
        return -1;
    }

    char target_path[256] = "";

    // Шаг 1: Ищем, какая именно папка hwmonX принадлежит процессору
    for (size_t i = 0; i < glob_result.gl_pathc; i++) {
        char name_path[256];
        char name_buf[64] = "";
        
        snprintf(name_path, sizeof(name_path), "%s/name", glob_result.gl_pathv[i]);
        FILE *f_name = fopen(name_path, "r");
        
        if (f_name) {
            if (fgets(name_buf, sizeof(name_buf), f_name)) {
                name_buf[strcspn(name_buf, "\r\n")] = '\0';
            }
            fclose(f_name);
        }

        // Драйверы процессоров: coretemp (Intel) или k10temp (AMD)
        if (strcmp(name_buf, "coretemp") == 0 || strcmp(name_buf, "k10temp") == 0) {
            strncpy(target_path, glob_result.gl_pathv[i], sizeof(target_path) - 1);
            break; 
        }
    }

    // Если процессорный hwmon не найден, освобождаем glob и выходим
    if (strlen(target_path) == 0) {
        globfree(&glob_result);
        return -1;
    }

    // Шаг 2: Внутри найденной папки ищем все доступные датчики температуры (temp*_input)
    glob_t temp_glob;
    char temp_pattern[300];
    snprintf(temp_pattern, sizeof(temp_pattern), "%s/temp*_input", target_path);

    if (glob(temp_pattern, 0, nullptr, &temp_glob) == 0) {
        zone_count = temp_glob.gl_pathc;
        if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;

        for (int i = 0; i < zone_count; i++) {
            // Открываем файл температуры и оставляем дескриптор в памяти
            g_temp_files[i] = fopen(temp_glob.gl_pathv[i], "r");

            // Пытаемся прочитать человеческое имя датчика (например, temp1_label -> "Package id 0")
            char label_path[350];
            strncpy(label_path, temp_glob.gl_pathv[i], sizeof(label_path) - 1);
            
            // Подменяем "_input" на "_label" в пути к файлу
            char *sub = strstr(label_path, "_input");
            if (sub) {
                strcpy(sub, "_label");
            }

            FILE *f_label = fopen(label_path, "r");
            if (f_label) {
                if (fgets(g_zone_names[i], sizeof(g_zone_names[i]), f_label)) {
                    g_zone_names[i][strcspn(g_zone_names[i], "\r\n")] = '\0';
                } else {
                    snprintf(g_zone_names[i], sizeof(g_zone_names[i]), "Core %d", i);
                }
                fclose(f_label);
            } else {
                // Если файла label нет (на AMD часто бывает), пишем просто Core X
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
        // Копируем имя из кэша
        strncpy(payload.zones[i].name, g_zone_names[i], sizeof(payload.zones[i].name) - 1);

        long temp_raw = -1;
        if (g_temp_files[i]) {
            rewind(g_temp_files[i]);
            if (fscanf(g_temp_files[i], "%ld", &temp_raw) != 1) {
                temp_raw = -1;
            }
        }

        // hwmon отдает температуру в миллиградусах (например, 45000 вместо 45), делим на 1000
        payload.zones[i].temp = (temp_raw != -1) ? (int)(temp_raw / 1000) : -1;
    }
    return payload;
}

void free_thermal(void) {
    if (initialized) {
        for (int i = 0; i < zone_count; i++) {
            if (g_temp_files[i]) {
                fclose(g_temp_files[i]);
                g_temp_files[i] = nullptr;
            }
        }
        initialized = 0;
        zone_count = 0;
    }
}

