#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/process.h"

int stop(const char *site_name) {
    char pid_file[256];

    snprintf(pid_file, sizeof(pid_file), "/var/run/serverview/%s.pid", site_name);

    return stop_process(site_name, pid_file);
}