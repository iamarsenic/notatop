#pragma once

#include <ncurses.h>
#include "thermal.h"
#include "meminfo.h"

typedef struct {
    WINDOW *thermal_win;
    WINDOW *mem_win;
    WINDOW *rpm_win;
    int thermal_height;
} TUI_Layout;

void init_tui_graphics(void);
TUI_Layout create_tui_layout(int active_zones);
void render_tui_frame(const TUI_Layout *layout, const ThermalPayload *thermal, const SystemMemory *mem, long fan_rpm);
void destroy_tui_layout(TUI_Layout *layout);
