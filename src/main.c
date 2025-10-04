// Import standard libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// Import project's libs
#include "lib/color.h"
#include "command/setup.h"
#include "command/start-all.h"
#include "command/stop-all.h"
#include <linux/limits.h>

// Program entry point, aka main function
int main(int argc, char *argv[]) {
    char *command = NULL;
    int help_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            help_flag = 1;
        } else if (argv[i][0] != '-') {
            if (command == NULL) {
                command = argv[i];
            }
        }
    }

    if (help_flag) {
        if (command == NULL) {
            printf(BOLD_CYAN "Usage: %s <command> [options]\n\n" COLOR_RESET, argv[0]);
            printf("Commands:\n");
            printf("  setup      Set up the environment\n");
            printf("  start-all  Start all enabled sites\n");
            printf("  stop-all   Stop all enabled sites\n");
            printf("\nFlags:\n");
            printf("  " BOLD_YELLOW "--help" COLOR_RESET "  Display this help message\n");
        } else if (strcmp(command, "setup") == 0) {
            printf("Usage: %s setup\n\n", argv[0]);
            printf("Set up the environment for the server view CLI.\n");
        } else if (strcmp(command, "start-all") == 0) {
            printf("Usage: %s start-all\n\n", argv[0]);
            printf("Start all enabled sites.\n");
        } else if (strcmp(command, "stop-all") == 0) {
            printf("Usage: %s stop-all\n\n", argv[0]);
            printf("Stop all enabled sites.\n");
        } else {
            fprintf(stderr, BOLD_RED "Help for command '%s' is not yet implemented.\n" COLOR_RESET, command);
        }
    } else if (command == NULL) {
        fprintf(stderr, BOLD_RED "Usage: %s <command> [options]\n" COLOR_RESET, argv[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        return 1;
    } else if (strcmp(command, "setup") == 0) {
        char resolved_path[PATH_MAX];
        if (realpath(argv[0], resolved_path) == NULL) {
            perror("Failed to resolve executable path");
            return 1;
        }
        setup(resolved_path);
    } else if (strcmp(command, "start-all") == 0) {
        start_all();
    } else if (strcmp(command, "stop-all") == 0) {
        stop_all();
    } else {
        fprintf(stderr, BOLD_RED "%s: '%s' is not a %s command. See '%s --help'.\n" COLOR_RESET, argv[0], command, argv[0], argv[0]);
        return 1;
    }

    return 0;
}
