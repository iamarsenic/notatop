#define _GNU_SOURCE
#include "../include/ui.h"
#include "../include/cpufreq.h"
#include "../include/loadaverage.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

#define NOTATOP_VERSION "1.18.1"

// Helper to format bytes into human-readable units
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

// Universal helper function to reset window borders and inject titles safely
static void reset_window_frame(WINDOW *win, const char *title) {
    if (win == nullptr) return;
    werase(win);
    box(win, 0, 0);
    if (title && title[0] != '\0') {
        mvwprintw(win, 0, 3, "%s", title);
    }
}

// Universal function to initialize an ncurses window block with a border and title
static WINDOW *init_window_block(int height, int width, int y, int x, const char *title) {
    WINDOW *win = newwin(height, width, y, x);
    if (win == nullptr) return nullptr;

    reset_window_frame(win, title);
    return win;
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
    
    // Fetch current time for the main header title
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    char title_buf[64] = {}; 
    snprintf(title_buf, sizeof(title_buf), "NotATop v%s | %02d:%02d:%02d", 
             NOTATOP_VERSION, t->tm_hour, t->tm_min, t->tm_sec);

    // Left column layout configurations (X = 5, Width = 40)
    layout.thermal_height = 10; 
    layout.thermal_win    = init_window_block(10, 40, 2, 5, title_buf); 
    layout.load_win       = init_window_block(3, 40, 13, 5, "System Load (LA)"); 
    
    // Right column layout configurations (X = 48, Width = 60)
    layout.mem_win     = init_window_block(7, 60, 2, 48, "Memory");          
    layout.freq_win    = init_window_block(4, 60, 10, 48, "CPU Core Frequencies"); 
    layout.rpm_win     = init_window_block(4, 60, 15, 48, "Cooling / Fan");        
    
    return layout;
}

void render_tui_frame(const TUI_Layout *layout, const ThermalPayload *thermal, 
                      const SystemMemory *mem, const CPUFreqPayload *cpufreq, long fan_rpm) {
    
    // Fetch current time for top bar dynamic clock update
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    char time_str[64] = {}; 
    snprintf(time_str, sizeof(time_str), " NotATop v%s | %02d:%02d:%02d ", NOTATOP_VERSION, t->tm_hour, t->tm_min, t->tm_sec);

    // Reset layout blocks using standard static C helper functions
    reset_window_frame(layout->thermal_win, time_str);
    reset_window_frame(layout->load_win, " System Load (LA) ");
    reset_window_frame(layout->mem_win, " Memory ");
    reset_window_frame(layout->freq_win, " CPU Core Frequencies ");
    reset_window_frame(layout->rpm_win, " Cooling / Fan ");

    // --- 1. Render Thermal Zones Data ---
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

    // --- 2. Render Load Average Data ---
    LoadAvg load = get_system_load();
    if (load.success) {
        mvwprintw(layout->load_win, 1, 2, "Load Average: %.2f/%.2f%.2f", load.min_1, load.min_5, load.min_15);
    } else {
        mvwprintw(layout->load_win, 1, 2, "LA metrics error");
    }

    // --- 3. Render Memory Allocation Data ---
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

    // --- 4. Render CPU Frequencies ---
    for (int i = 0; i < cpufreq->count && i < 4; i++) {
        mvwprintw(layout->freq_win, 1, 2 + (i * 14), "CPU%d: %4ldM", i, cpufreq->freqs[i]);
    }

    // --- 5. Render Cooling Fan RPM ---
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
    
    /* refreshing all windows */
	wnoutrefresh(stdscr);
	wnoutrefresh(layout->thermal_win); 
	wnoutrefresh(layout->load_win); 
	wnoutrefresh(layout->mem_win); 
	wnoutrefresh(layout->freq_win);
	wnoutrefresh(layout->rpm_win);
	doupdate(); /* i love this */
}
void destroy_tui_layout(TUI_Layout *layout) {
    delwin(layout->thermal_win); 
    delwin(layout->load_win); 
    delwin(layout->mem_win);     delwin(layout->freq_win);    
    delwin(layout->rpm_win);
}

