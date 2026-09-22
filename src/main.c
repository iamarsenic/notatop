#define MAX_ZONES 10
#define NOTATOP_VERSION "1.16.0" // pneumonoultramicroscopicsilicovolcanoconiosis

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <glob.h>
#include <termios.h>
#include <signal.h>
#include <ncurses.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "../include/read.h"
#include "../include/meminfo.h"
#include "../include/logger.h"

volatile sig_atomic_t keep_running = 1;

// this needs because i dont know how B in KB
void format_bytes(long kb, char *dest, size_t dest_size) {
	double value = (double)kb;

	const char *units[] = {"kB", "MB", "GB", "TB"};
	int i = 0;

	while (value >= 1024.0 && i < 3) {
		value /= 1024.0;
		i++;
	}

	snprintf(dest, dest_size, "%.4f %s", value, units[i]);
}

// use CTRL + C
void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

typedef struct {
    char type_path[256];
    char temp_path[256];
    char name[64];
    int temp;
} ThermalZone;

typedef struct {
    WINDOW *win;
    int width;
    int height;
    int start_x;
    int start_y;
} MonitorWindow;

MonitorWindow thermal_win;
MonitorWindow mem_win;

int main() {
	init_logging();

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    glob_t g_zones = {0};
	size_t zone_count = 0;
	int glob_success = 0;

	// so smart thing so smart does smart things
    if (glob("/sys/class/thermal/thermal_zone*", 0, NULL, &g_zones) == 0) {
		glob_success = 1;
		zone_count = g_zones.gl_pathc;
		if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;
	} else {
		fprintf(stderr, "[!] Failed to find thermal zones\n");
    }

    ThermalZone zones[MAX_ZONES];

    for (size_t i = 0; i < zone_count; i++) {
        snprintf(zones[i].type_path, sizeof(zones[i].type_path), "%s/type", g_zones.gl_pathv[i]);
        snprintf(zones[i].temp_path, sizeof(zones[i].temp_path), "%s/temp", g_zones.gl_pathv[i]);
    }

    // Initialisation...
    initscr();      // NCurses start
    clear();        // Clear terminal
    cbreak();       // Line buffering disabled
    noecho();       // Don't echo input
    curs_set(0);    // Hiding cursor

	// new minor update: i added this.. just watch
	if (has_colors()) {
        start_color();
		use_default_colors(); 
		
		init_pair(4, -1, -1); // hyprland users starting use my util
		
        // (70-79°C)
        init_pair(1, COLOR_YELLOW, -1); // yellow is normal btw
        // (80-89°C)
        init_pair(2, COLOR_RED, -1); // red means be accurated with compilation
        // (90-110°C)
        init_pair(3, COLOR_BLACK, COLOR_RED); // black means you pc is lava
    }

    int win_height = (int)zone_count + 4;
    if (win_height < 8) win_height = 8;

    // Thermal zones window  
	int win_h, win_w;
    thermal_win.height  = win_height;
    thermal_win.width   = 40;
	thermal_win.start_y = 2 + win_height;
    thermal_win.start_x = 5;
    WINDOW *thermal_win_str  = newwin(thermal_win.height, thermal_win.width, thermal_win.start_y, thermal_win.start_x);

	// Mem window
	mem_win.height	 = win_height;
	mem_win.width	 = 60;
	mem_win.start_y	 = 2 + win_height;
	mem_win.start_x	 = 45;
	WINDOW *memwin	 = newwin(mem_win.height, mem_win.width, mem_win.start_y, mem_win.start_x);

	wtimeout(thermal_win_str, 100);
	wtimeout(memwin, 100);

	// Memory
	SystemMemory mem;

    keypad(stdscr, TRUE);


	// Writing beatiful text
	mvwprintw(memwin, 1, 2, "Used/Total:");
	mvwprintw(memwin, 2, 2, "Free/Cached:");
	mvwprintw(memwin, 3, 2, "Available:");
	mvwprintw(memwin, 4, 2, "Mapped:");
	mvwprintw(memwin, 5, 2, "Active/Inactive:");

	while(keep_running) {
        werase(thermal_win_str);

        box(thermal_win_str, 0, 0);
        werase(thermal_win_str);

        box(thermal_win_str, 0, 0);
		box(memwin, 0, 0);
        
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
		
		// in new patch i added some thinks... newermind!
		mvwprintw(thermal_win_str, 0, 3, " NotATop v%s | %s ", NOTATOP_VERSION, time_str);

        for (size_t i = 0; i < zone_count; i++) {
            read_string(zones[i].type_path, &zones[i].name, 1, sizeof(zones[i].name));
            long temp_raw = read_long(zones[i].temp_path);

            if (temp_raw != -1) {
                int temp_c = (int)(temp_raw / 1000.0);
                mvwprintw(thermal_win_str, 1 + (int)i, 2, "[%zu] %-25s", i, zones[i].name);

                // --- COLORIZED LOGIC ---
                int color_pair = 0;
                if (temp_c >= 70 && temp_c < 80) {
                    color_pair = 1; // yellow
                } else if (temp_c >= 80 && temp_c < 90) {
                    color_pair = 2; // red
                } else if (temp_c >= 90 && temp_c <= 110) {
                    color_pair = 3; // black on red
                }

                // plz use colorized terminals
                if (color_pair > 0) {
                    if (color_pair == 3) {
						// turn the fat fucking pig on
                        wattron(thermal_win_str, COLOR_PAIR(color_pair) | A_BOLD);
                    } else {
                        wattron(thermal_win_str, COLOR_PAIR(color_pair));
                    }
                }

                // wow, this thing printing temperature
                mvwprintw(thermal_win_str, 1 + (int)i, 35, "%3dC", temp_c);

                // turn off this shit
                if (color_pair > 0) {
                    if (color_pair == 3) {
						// turn the fat fucking pig off
                        wattroff(thermal_win_str, COLOR_PAIR(color_pair) | A_BOLD);
                    } else {
                        wattroff(thermal_win_str, COLOR_PAIR(color_pair));
                    }
                }
			}
        }

		if (parse_meminfo(&mem) == 0) {
			char total_str[32];
			char avail_str[32];
			char used_str[32];
			char mapped_str[32];
			char free_str[32];
			char cached_str[32];
			char active_str[32];
			char inactive_str[32];

			format_bytes(mem.total, total_str, sizeof(total_str));
			format_bytes(mem.available, avail_str, sizeof(avail_str));
			format_bytes(mem.mapped, mapped_str, sizeof(mapped_str));
			format_bytes(mem.free, free_str, sizeof(free_str));
			format_bytes(mem.cached, cached_str, sizeof(cached_str));
			format_bytes(mem.active, active_str, sizeof(active_str));
			format_bytes(mem.inactive, inactive_str, sizeof(inactive_str));

			long used_kb = mem.total - mem.available;
			format_bytes(used_kb, used_str, sizeof(used_str));
		
			mvwprintw(memwin, 1, 20, "%s/%s", used_str, total_str);
			mvwprintw(memwin, 2, 20, "%s/%s", free_str, cached_str);
			mvwprintw(memwin, 3, 20, "%s", avail_str);
			mvwprintw(memwin, 4, 20, "%s", mapped_str);
			mvwprintw(memwin, 5, 20, "%s/%s", active_str, inactive_str);
		} else {
			mvwprintw(memwin, 1, 2, "[!] Error: Failed to read meminfo: %s", strerror(errno));
			fprintf(stderr, "[!] Main: Failed to read meminfo: %s", strerror(errno));
		}

        mvwprintw(thermal_win_str, thermal_win.height - 2, 2, "Q to exit");

        wrefresh(thermal_win_str);
		wrefresh(memwin);

		// CTRL + C is better
		int ch = wgetch(thermal_win_str);
		if (ch == 'q') keep_running = 0;
    }

	// bye :(
    delwin(thermal_win_str);
	delwin(memwin);
    endwin();

    if (glob_success) {
		globfree(&g_zones);
    return EXIT_SUCCESS;
}

