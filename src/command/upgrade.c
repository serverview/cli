#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../lib/color.h"
#include "../lib/download.h"
#include "../lib/version.h"

// Forward declarations for functions defined in setup.c
extern int download_core(const char *version_tag);
extern int install_core();
extern int start_service();

static int stop_service() {
    printf(BOLD_CYAN "Stopping svcore service...\n" COLOR_RESET);
    if (system("systemctl stop svcore") != 0) {
        fprintf(stderr, BOLD_YELLOW "Failed to stop svcore service (maybe it was not running?).\n" COLOR_RESET);
    }
    printf(BOLD_GREEN "svcore service stopped.\n" COLOR_RESET);
    return 0;
}

int upgrade() {
    if (geteuid() != 0) {
        fprintf(stderr, BOLD_RED "This command must be run as root.\n" COLOR_RESET);
        return 1;
    }

    printf(BOLD_CYAN "Checking for core updates...\n" COLOR_RESET);

    char *installed_version = get_component_version("core");
    if (installed_version == NULL) {
        fprintf(stderr, BOLD_YELLOW "Core is not installed. Please run 'setup' first.\n" COLOR_RESET);
        return 1;
    }

    char *latest_version = get_latest_core_version();
    if (latest_version == NULL) {
        fprintf(stderr, BOLD_RED "Failed to get the latest core version.\n" COLOR_RESET);
        free(installed_version);
        return 1;
    }

    printf("Installed version: %s, Latest version: %s\n", installed_version, latest_version);

    if (strcmp(installed_version, latest_version) == 0) {
        printf(BOLD_GREEN "You are already on the latest version of the core.\n" COLOR_RESET);
        free(installed_version);
        free(latest_version);
        return 0;
    }

    printf(BOLD_CYAN "Upgrading core from %s to %s...\n" COLOR_RESET, installed_version, latest_version);

    stop_service();

    if (download_core(latest_version) != 0) {
        fprintf(stderr, BOLD_RED "Failed to download new version.\n" COLOR_RESET);
        start_service(); // Attempt to restart the service
        free(installed_version);
        free(latest_version);
        return 1;
    }

    if (install_core() != 0) {
        fprintf(stderr, BOLD_RED "Failed to install new version.\n" COLOR_RESET);
        start_service(); // Attempt to restart the service
        free(installed_version);
        free(latest_version);
        return 1;
    }

    if (save_component_version("core", latest_version) != 0) {
        fprintf(stderr, BOLD_YELLOW "Warning: Could not save new core version.\n" COLOR_RESET);
    }

    printf(BOLD_GREEN "Core upgraded successfully to %s.\n" COLOR_RESET, latest_version);

    start_service();

    free(installed_version);
    free(latest_version);

    return 0;
}