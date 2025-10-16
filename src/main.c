#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <linux/limits.h>
#include <curl/curl.h> // Include curl header

// Import project's libs
#include "lib/color.h"
#include "cli_version.h" // Include CLI version header

// Import command headers
#include "command/setup.h"
#include "command/start-all.h"
#include "command/stop-all.h"
#include "command/start.h"
#include "command/stop.h"
#include "command/status.h"
#include "command/upgrade.h"

// Program entry point, aka main function
int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    char *command = NULL;
    int help_flag = 0;
    int version_flag = 0; // Add version_flag

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            help_flag = 1;
        } else if (strcmp(argv[i], "--version") == 0) { // Check for --version flag
            version_flag = 1;
        } else if (argv[i][0] != '-') {
            if (command == NULL) {
                command = argv[i];
            }
        }
    }

    if (version_flag) {
        printf("Server View Controller version %s\n", CLI_VERSION);
        curl_global_cleanup();
        return 0;
    }

    if (help_flag) {
        if (command == NULL) {
            printf(BOLD_CYAN "Usage: %s <command> [options]\n\n" COLOR_RESET, argv[0]);
            printf("Commands:\n");
            printf("  setup      Set up the environment\n");
            printf("  upgrade    Upgrade the core to the latest version\n");
            printf("  start      Start a specific site by name\n");
            printf("  stop       Stop a specific site by name\n");
            printf("  status     Get the status of a specific site\n");
            printf("  start-all  Start all enabled sites\n");
            printf("  stop-all   Stop all enabled sites\n");
            printf("\nFlags:\n");
            printf("  " BOLD_YELLOW "--help" COLOR_RESET "  Display this help message\n");
            printf("  " BOLD_YELLOW "--version" COLOR_RESET " Display CLI version\n"); // Add --version to help
        } else if (strcmp(command, "setup") == 0) {
            printf("Usage: %s setup\n\n", argv[0]);
            printf("Set up the environment for the server view CLI.\n");
        } else if (strcmp(command, "upgrade") == 0) {
            printf("Usage: %s upgrade\n\n", argv[0]);
            printf("Upgrade the core to the latest version.\n");
        } else if (strcmp(command, "start-all") == 0) {
            printf("Usage: %s start-all\n\n", argv[0]);
            printf("Start all enabled sites.\n");
        } else if (strcmp(command, "stop-all") == 0) {
            printf("Usage: %s stop-all\n\n", argv[0]);
            printf("Stop all enabled sites.\n");
        } else if (strcmp(command, "start") == 0) {
            printf("Usage: %s start <site-name>\n\n", argv[0]);
            printf("Start the specified site.\n");
        } else if (strcmp(command, "stop") == 0) {
            printf("Usage: %s stop <site-name>\n\n", argv[0]);
            printf("Stop the specified site.\n");
        } else if (strcmp(command, "status") == 0) {
            printf("Usage: %s status <site-name>\n\n", argv[0]);
            printf("Get the status of the specified site.\n");
        } else {
            fprintf(stderr, BOLD_RED "Help for command '%s' is not yet implemented.\n" COLOR_RESET, command);
        }
    } else if (command == NULL) {
        fprintf(stderr, BOLD_RED "Usage: %s <command> [options]\n" COLOR_RESET, argv[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        curl_global_cleanup();
        return 1;
    } else if (strcmp(command, "setup") == 0) {
        char resolved_path[PATH_MAX];
        if (realpath(argv[0], resolved_path) == NULL) {
            perror("Failed to resolve executable path");
            curl_global_cleanup();
            return 1;
        }
        setup(resolved_path);
    } else if (strcmp(command, "upgrade") == 0) {
        char resolved_path[PATH_MAX];
        if (realpath(argv[0], resolved_path) == NULL) {
            perror("Failed to resolve executable path");
            curl_global_cleanup();
            return 1;
        }
        upgrade(resolved_path);
    } else if (strcmp(command, "start-all") == 0) {
        start_all();
    } else if (strcmp(command, "stop-all") == 0) {
        stop_all();
    } else if (strcmp(command, "start") == 0) {
        if (argc < 3) {
            fprintf(stderr, BOLD_RED "Error: Missing site name for start command.\n" COLOR_RESET);
            curl_global_cleanup();
            return 1;
        }
        start(argv[2]);
    } else if (strcmp(command, "stop") == 0) {
        if (argc < 3) {
            fprintf(stderr, BOLD_RED "Error: Missing site name for stop command.\n" COLOR_RESET);
            curl_global_cleanup();
            return 1;
        }
        stop(argv[2]);
    } else if (strcmp(command, "status") == 0) {
        if (argc < 3) {
            fprintf(stderr, BOLD_RED "Error: Missing site name for status command.\n" COLOR_RESET);
            curl_global_cleanup();
            return 1;
        }
        status(argv[2]);
    } else {
        fprintf(stderr, BOLD_RED "%s: '%s' is not a %s command. See '%s --help'.\n" COLOR_RESET, argv[0], command, argv[0], argv[0]);
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}