#define _GNU_SOURCE
#include "../include/ui.h"
#include "../include/cpufreq.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

#define NOTATOP_VERSION "1.17.0"

static void format_bytes(long kb, char *dest, size_t dest_size) {
    double value = (double)kb;
    const char *units[] = {"kB", "MB", "GB", "TB"};
    int i = 0;
    while (value >= 1024.0 && i < 3) {
        value /= 1024.0;
        i++;
    }
    snprintf(dest, dest_size, "%.4f %s", value, units[i]);
}

void init_tui_graphics(void) {
    initscr(); clear(); cbreak(); noecho(); curs_set(0);
    if (has_colors()) {
        start_color(); use_default_colors();
        init_pair(1, COLOR_YELLOW, -1);       
        init_pair(2, COLOR_RED, -1);          
        init_pair(3, COLOR_BLACK, COLOR_RED); 
    }
    timeout(100);
    keypad(stdscr, TRUE);
}

TUI_Layout create_tui_layout(int active_zones) {
    (void)active_zones;
    TUI_Layout layout;
    
    layout.thermal_height = 17; 
    layout.thermal_win    = newwin(17, 40, 2, 5); 
    
    layout.mem_win     = newwin(7, 60, 2, 48);          
    layout.freq_win    = newwin(4, 60, 2 + 7 + 1, 48);  
    layout.rpm_win     = newwin(4, 60, 2 + 7 + 1 + 4 + 1, 48); 
    
    return layout;
}

void render_tui_frame(const TUI_Layout *layout, const ThermalPayload *thermal, 
                      const SystemMemory *mem, const CPUFreqPayload *cpufreq, long fan_rpm) {
    
    werase(layout->thermal_win); werase(layout->mem_win); 
    werase(layout->freq_win);    werase(layout->rpm_win);
    
    box(layout->thermal_win, 0, 0); box(layout->mem_win, 0, 0); 
    box(layout->freq_win, 0, 0);    box(layout->rpm_win, 0, 0);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

    mvwprintw(layout->thermal_win, 0, 3, " NotATop v%s | %s ", NOTATOP_VERSION, time_str);
    mvwprintw(layout->mem_win, 0, 3, " Memory ");
    mvwprintw(layout->freq_win, 0, 3, " CPU Core Frequencies ");
    mvwprintw(layout->rpm_win, 0, 3, " Cooling / Fan ");

    for (int i = 0; i < thermal->count && i < (layout->thermal_height - 3); i++) {
        int temp_c = thermal->zones[i].temp;
        mvwprintw(layout->thermal_win, 1 + i, 2, "[%d] %-25s", i, thermal->zones[i].name);

        if (temp_c != -1) {
            int pair = 0;
            if (temp_c >= 70 && temp_c < 80) pair = 1;
            else if (temp_c >= 80 && temp_c < 90) pair = 2;
            else if (temp_c >= 90) pair = 3;

            if (pair > 0) {
                if (pair == 3) wattron(layout->thermal_win, COLOR_PAIR(pair) | A_BOLD);
                else wattron(layout->thermal_win, COLOR_PAIR(pair));
            }
            mvwprintw(layout->thermal_win, 1 + i, 33, "%3d C", temp_c);
            if (pair > 0) {
                if (pair == 3) wattroff(layout->thermal_win, COLOR_PAIR(pair) | A_BOLD);
                else wattroff(layout->thermal_win, COLOR_PAIR(pair));
            }
        }
    }

    char t_str[32], a_str[32], u_str[32], m_str[32], f_str[32], c_str[32], ac_str[32], ic_str[32];
    format_bytes(mem->total, t_str, sizeof(t_str));
    format_bytes(mem->available, a_str, sizeof(a_str));
    format_bytes(mem->mapped, m_str, sizeof(m_str));
    format_bytes(mem->free, f_str, sizeof(f_str));
    format_bytes(mem->cached, c_str, sizeof(c_str));
    format_bytes(mem->active, ac_str, sizeof(ac_str));
    format_bytes(mem->inactive, ic_str, sizeof(ic_str));
    format_bytes(mem->total - mem->available, u_str, sizeof(u_str));

    mvwprintw(layout->mem_win, 1, 2, "Used/Total:");
    mvwprintw(layout->mem_win, 2, 2, "Free/Cached:");
    mvwprintw(layout->mem_win, 3, 2, "Available:");
    mvwprintw(layout->mem_win, 4, 2, "Mapped:");
    mvwprintw(layout->mem_win, 5, 2, "Active/Inactive:");

    mvwprintw(layout->mem_win, 1, 20, "%s/%s", u_str, t_str);
    mvwprintw(layout->mem_win, 2, 20, "%s/%s", f_str, c_str);
    mvwprintw(layout->mem_win, 3, 20, "%s", a_str);
    mvwprintw(layout->mem_win, 4, 20, "%s", m_str);
    mvwprintw(layout->mem_win, 5, 20, "%s/%s", ac_str, ic_str);

    for (int i = 0; i < cpufreq->count && i < 4; i++) {
        mvwprintw(layout->freq_win, 1, 2 + (i * 14), "CPU%d: %4ldM", i, cpufreq->freqs[i]);
    }

    mvwprintw(layout->rpm_win, 1, 2, "Fan Speed:");
    if (fan_rpm > 0) {
        wattron(layout->rpm_win, A_BOLD);
        mvwprintw(layout->rpm_win, 1, 20, "%ld RPM", fan_rpm);
        wattroff(layout->rpm_win, A_BOLD);
    } else if (fan_rpm == 0) {
        mvwprintw(layout->rpm_win, 1, 20, "IDLE (0 RPM)");
    } else {
        mvwprintw(layout->rpm_win, 1, 20, "[Unsupported]");
    }

    mvwprintw(layout->thermal_win, layout->thermal_height - 2, 2, "Q to exit");
    
    wrefresh(stdscr);
    wrefresh(layout->thermal_win); 
    wrefresh(layout->mem_win); 
    wrefresh(layout->freq_win);
    wrefresh(layout->rpm_win);
}

void destroy_tui_layout(TUI_Layout *layout) {
    delwin(layout->thermal_win); delwin(layout->mem_win); 
    delwin(layout->freq_win);    delwin(layout->rpm_win);
}

