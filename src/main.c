#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>

#include "../include/thermal.h"
#include "../include/fan.h"
#include "../include/meminfo.h"
#include "../include/cpufreq.h"
#include "../include/ui.h"
#include "../include/logger.h"

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    init_logging();

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    int active_zones = init_thermal();
    if (active_zones < 0) {
        active_zones = 0;
    }

    int cpu_cores = init_cpufreq();
    if (cpu_cores < 0) {
        cpu_cores = 0;
    }

    init_tui_graphics();
    TUI_Layout layout = create_tui_layout(active_zones);

    wtimeout(layout.thermal_win, 100);

    SystemMemory mem;

    while (keep_running) {
        ThermalPayload thermal = update_thermal();
        CPUFreqPayload cpufreq = update_cpufreq();
        long fan_rpm           = get_fan_rpm();
        parse_meminfo(&mem);

        render_tui_frame(&layout, &thermal, &mem, &cpufreq, fan_rpm);

        int ch = wgetch(layout.thermal_win);
        if (ch == 'q' || ch == 'Q') {
            keep_running = 0;
        }
    }

    destroy_tui_layout(&layout);
    endwin(); 
    free_thermal();
    free_cpufreq();
    
    return EXIT_SUCCESS;
}

