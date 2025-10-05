#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For readlink
#include <limits.h> // For PATH_MAX
#include "../lib/process.h"

int start(const char *site_name) {
    char symlink_path[PATH_MAX];
    snprintf(symlink_path, sizeof(symlink_path), "/etc/serverview/sites-enabled/%s", site_name);

    char config_path[PATH_MAX];
    ssize_t len = readlink(symlink_path, config_path, sizeof(config_path) - 1);
    if (len != -1) {
        config_path[len] = '\0'; // Null-terminate the path
    } else {
        // If readlink fails, assume it's not a symlink and use the original path
        strncpy(config_path, symlink_path, sizeof(config_path) - 1);
        config_path[sizeof(config_path) - 1] = '\0';
    }

    char pid_file[256];
    snprintf(pid_file, sizeof(pid_file), "/var/run/serverview/%s.pid", site_name);

    return start_process(site_name, config_path, pid_file);
}