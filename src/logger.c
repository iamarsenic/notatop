#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define MAX_LOG_SIZE (100 * 1024 * 1024)

typedef struct {
	int xdg;
	int home;
} HaveStatus;

void init_logging() {
	HaveStatus have = {0, 0};
	char log_dir[512];
	char log_path[512];

	const char *xdg_data = getenv("XDG_DATA_HOME");
	const char *home = getenv("HOME");

	if (home && home[0] != '\0') {
		have.home = 1;
	}

	if (xdg_data && xdg_data[0] != '\0') {
		have.xdg = 1;
	}

	if (!have.home) {
		fprintf(stderr, "[!] $HOME var not found. Using /tmp/notatop/log.\n");
		snprintf(log_dir, sizeof(log_dir), "%s", "/tmp/notatop/log");
	} else if (have.xdg) {
		snprintf(log_dir, sizeof(log_dir), "%s/notatop", xdg_data);
	} else {
		fprintf(stderr, "[.] $XDG_HOME_DIR is not set. Using $HOME/.local/state/notatop\n");
		snprintf(log_dir, sizeof(log_dir), "%s/.local/state/notatop/log", home);
	}

	if (have.home) {
		char tmp[512];
		snprintf(tmp, sizeof(tmp), "%s/.local", home);
		mkdir(tmp, 0755);
		snprintf(tmp, sizeof(tmp), "%s/.local/state", home);
		mkdir(tmp, 0755);
		snprintf(tmp, sizeof(tmp), "%s/.local/state/notatop", home);
		mkdir(tmp, 0755);
	}
	mkdir(log_dir, 0755);

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char time_str[64];
	strftime(time_str, sizeof(time_str), "%Y-%m-%d_%H:%M:%S", t);

	snprintf(log_path, sizeof(log_path), "%s/notatop_%s.log", log_dir, time_str);

	int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd != -1) {
		dup2(fd, STDERR_FILENO);
		close(fd);
	}

	fprintf(stderr, "[+] Debugger initialiation started\n");
}
