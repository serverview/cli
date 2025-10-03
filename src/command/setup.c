#include "lib/color.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int setup() {
    printf(BOLD_CYAN "Setting up the environment...\n" COLOR_RESET);
    printf(BOLD_CYAN "Creating necessary directories...\n" COLOR_RESET);

    char directories[255][255] = {
        "/var/www/svh",
        "/etc/serverview",
        "/etc/serverview/site-enabled",
        "/etc/serverview/site-available",
        "/var/log/serverview",
        "/var/serverview",
        "/var/serverview/default"
    };

    for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
        if (mkdir(directories[i], 0755) != 0) {
            fprintf(stderr, BOLD_RED "Error creating directory %s: %s\n" COLOR_RESET, directories[i], strerror(errno));
        }
    }

    return 0;
}