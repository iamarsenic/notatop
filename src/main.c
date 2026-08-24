#define MAX_ZONES 10

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

volatile sig_atomic_t keep_running = 1;

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

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

typedef struct {
    char type_path[256];
    char temp_path[256];
    char name[128];
    int temp;
} ThermalZone;

typedef struct {
    WINDOW *win;
    int width;
    int height;
    int start_x;
    int start_y;
} MonitorWindow;

MonitorWindow temp_win;
MonitorWindow type_win;
MonitorWindow mem_win;

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    glob_t g_zones;
    // Ищем сами папки thermal_zone*, чтобы дальше гибко цеплять файлы внутри них
    if (glob("/sys/class/thermal/thermal_zone*", 0, NULL, &g_zones) != 0) {
        fprintf(stderr, "[!] Failed to find thermal zones\n");
    }

    size_t zone_count = g_zones.gl_pathc;
    if (zone_count > MAX_ZONES) zone_count = MAX_ZONES;

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

    // Фиксируем нормальную высоту окон, чтобы всё влезало и не было артефактов
    int win_height = (int)zone_count + 4;
    if (win_height < 8) win_height = 8;

    // Thermal zones window  
    // Temp window
    temp_win.height  = win_height;
    temp_win.width   = 20;
    temp_win.start_y = 2;
    temp_win.start_x = 45;
    WINDOW *tempwin  = newwin(temp_win.height, temp_win.width, temp_win.start_y, temp_win.start_x);
    
    // Type window
    type_win.height  = win_height;
    type_win.width   = 40;
	type_win.start_y = 2;
    type_win.start_x = 5;
    WINDOW *typewin  = newwin(type_win.height, type_win.width, type_win.start_y, type_win.start_x);

	// Mem window
	mem_win.height	 = win_height;
	mem_win.width	 = 60;
	mem_win.start_y	 = 2;
	mem_win.start_x	 = 65;
	WINDOW *memwin	 = newwin(mem_win.height, mem_win.width, mem_win.start_y, mem_win.start_x);

	wtimeout(tempwin, 100);
    wtimeout(typewin, 100);
	wtimeout(memwin, 100);

	// Memory
	SystemMemory mem;

    keypad(stdscr, TRUE);
    
    while(keep_running) {
        werase(tempwin);
        werase(typewin);
		werase(memwin);

        box(tempwin, 0, 0);
        box(typewin, 0, 0);
		box(memwin, 0, 0);
        
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
        
        mvwprintw(typewin, 0, 3, " NotATop v1.1 | %s ", time_str);
        mvwprintw(typewin, 0, 30, " Thermal ");
		mvwprintw(memwin, 0, 3, " Memory Info ");

        for (size_t i = 0; i < zone_count; i++) {
            read_string(zones[i].type_path, (char (*)[64])zones[i].name, 1, 64);

            long temp_raw = read_long(zones[i].temp_path);

            if (temp_raw != -1) {
                int temp_c = (int)(temp_raw / 1000.0);
                mvwprintw(typewin, 1 + (int)i, 2, "[%zu] %s", i, zones[i].name);
                mvwprintw(tempwin, 1 + (int)i, 2, "[%zu] %d°C", i, temp_c);
            }
        }

		if (parse_meminfo(&mem) == 0) {
			char total_str[32], avail_str[32], used_str[32];

			format_bytes(mem.mem_total, total_str, sizeof(total_str));
			format_bytes(mem.mem_available, avail_str, sizeof(avail_str));

			long used_kb = mem.mem_total - mem.mem_available;
			format_bytes(used_kb, used_str, sizeof(used_str));

			mvwprintw(memwin, 1, 2, "Total:\t%s", total_str);
			mvwprintw(memwin, 2, 2, "Used:\t\t%s", used_str);
			mvwprintw(memwin, 3, 2, "Available:\t%s", avail_str);
		} else {
			mvwprintw(memwin, 1, 2, "[!] Error: Failed to read meminfo: %s", strerror(errno));
		}
        mvwprintw(typewin, type_win.height - 2, 2, "Ctrl+C to exit");
        
        wrefresh(tempwin);
        wrefresh(typewin);
		wrefresh(memwin);
    }

    delwin(tempwin);
    delwin(typewin);
	delwin(memwin);
    endwin();

    globfree(&g_zones);
    printf("Exiting...\n");
    return EXIT_SUCCESS;
}
