// Import standard libs
#include <stdio.h>
#include <string.h>

// Import project's libs
#include "lib/color.h"

// Program entry point, aka main function
int main(int argc, char *argv[]) {
    char *executable = argv[0];
    if(argc < 2) {
        printf("No command specified, please type '%s help' to get a list of all commands.\n", executable);
        return 0;
    }

    char *command = argv[1];

    if(strcmp(command, "help") == 0) {
        printf(
            BOLD_CYAN "Welcome to Server View Control, the cli to manage them all.\n\n" COLOR_RESET
            BOLD_YELLOW "'help'" COLOR_RESET " - Get the list of all commands.\n"
        );
    } else {
        printf(BOLD_RED "Command '%s' not found" COLOR_RESET ", please use the 'help' command to get a list of all commands.\n", command);
    }

    return 0;
}