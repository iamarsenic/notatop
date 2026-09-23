#include "../include/fan.h"
#include <stdio.h>
#include <glob.h>
#include <string.h>

long get_fan_rpm(void) {
    // Статические переменные сохраняют состояние между вызовами функции
    static FILE *f_rpm = nullptr;
    static bool initialized = false;
    static bool init_failed = false;

    // Если в прошлый раз папка кулера вообще не нашлась, не тратим время
    if (init_failed) {
        return -1;
    }

    // Инициализация: выполняется ровно ОДИН РАЗ при самом первом вызове
    if (!initialized) {
        glob_t glob_result;
        // Ищем актуальный путь к fan1_input, игнорируя прыгающие индексы hwmonX
        if (glob("/sys/devices/platform/thinkpad_hwmon/hwmon/hwmon*/fan1_input", 0, nullptr, &glob_result) == 0) {
            // Открываем файл и оставляем дескриптор в памяти программы
            f_rpm = fopen(glob_result.gl_pathv[0], "r");
            initialized = true;
        }
        globfree(&glob_result);

        // Если файл не открылся (например, нет прав или модуля в ядре)
        if (!f_rpm) {
            init_failed = true;
            return -1;
        }
    }

    long val = -1;

    // Мгновенный сброс указателя чтения в начало файла без переоткрытия
    rewind(f_rpm);
    
    // Ядро Linux само обновит значение в виртуальном файле при парсинге
    if (fscanf(f_rpm, "%ld", &val) != 1) {
        val = -1;
    }

    return val;
}

