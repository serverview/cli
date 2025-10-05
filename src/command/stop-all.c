#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/process.h"
#include "stop-all.h"

int stop_all() {
    DIR *dir = opendir("/var/run/serverview");
    if (!dir) {
        perror("Failed to open /var/run/serverview directory");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry is a .pid file
        if (strstr(entry->d_name, ".pid") != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char pid_file[512];
            snprintf(pid_file, sizeof(pid_file), "/var/run/serverview/%s", entry->d_name);

            // Remove .pid from the name for display
            char site_name[256];
            strncpy(site_name, entry->d_name, sizeof(site_name) - 1);
            site_name[sizeof(site_name) - 1] = '\0';
            char *dot = strstr(site_name, ".pid");
            if (dot) {
                *dot = '\0';
            }

            stop_process(site_name, pid_file);
        }
    }

    closedir(dir);
    return 0;
}
