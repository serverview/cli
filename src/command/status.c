#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/process.h"

int status(const char *site_name) {
    char pid_file[256];

    snprintf(pid_file, sizeof(pid_file), "/var/run/serverview/%s.pid", site_name);

    return get_process_status(site_name, pid_file);
}
