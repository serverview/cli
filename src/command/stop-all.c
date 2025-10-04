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

            printf("Stopping site: %s\n", entry->d_name);
            stop_process(pid_file);
        }
    }

    closedir(dir);
    return 0;
}
