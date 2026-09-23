#include "../include/fan.h"
#include <stdio.h>

long get_fan_rpm(void) {
    FILE *fl = fopen("/sys/devices/platform/thinkpad_hwmon/hwmon/hwmon9/fan1_input", "r");
    if (!fl) return -1;
    long val = -1;
    if (fscanf(fl, "%ld", &val) != 1) val = -1;
    fclose(fl);
    return val;
}
