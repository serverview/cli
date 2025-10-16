#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // For readlink
#include <limits.h> // For PATH_MAX
#include "../lib/process.h"
#include "start-all.h"

int start_all() {
    DIR *dir = opendir("/etc/serverview/sites-enabled");
    if (!dir) {
        perror("Failed to open sites-enabled directory");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_LNK || entry->d_type == DT_REG) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char symlink_path[PATH_MAX];
            snprintf(symlink_path, sizeof(symlink_path), "/etc/serverview/sites-enabled/%s", entry->d_name);

            char config_path[PATH_MAX];
            ssize_t len = readlink(symlink_path, config_path, sizeof(config_path) - 1);
            if (len == -1) {
                perror("Failed to read symlink");
                continue; // Skip this entry
            }
            config_path[len] = '\0'; // Null-terminate the path

            char pid_file[PATH_MAX];
            snprintf(pid_file, sizeof(pid_file), "/var/run/serverview/%s.pid", entry->d_name);

            if (start_process(entry->d_name, config_path, pid_file) == -1) {
                fprintf(stderr, "Error: Failed to start site %s.\n", entry->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}