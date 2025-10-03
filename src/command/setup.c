// Import standard libs
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// Import project's libs
#include "../lib/color.h"
#include "../lib/fs.h"

int create_directories() {
    printf(BOLD_CYAN "Creating necessary directories...\n" COLOR_RESET);

    const char *directories[] = {
        "/var/www/svh",
        "/etc/serverview",
        "/etc/serverview/sites-enabled",
        "/etc/serverview/sites-available",
        "/var/log/serverview",
        "/var/serverview/default"
    };
    int num_directories = sizeof(directories) / sizeof(directories[0]);

    for (int i = 0; i < num_directories; i++) {
        if (make_directory(directories[i]) != 0) {
            fprintf(stderr, BOLD_RED "Error creating directory %s: %s\n" COLOR_RESET, directories[i], strerror(errno));
            return 1;
        }
    }
    return 0;
}

int create_default_config_file() {
    printf(BOLD_CYAN "Creating config file...\n" COLOR_RESET);
    FILE *config_file = fopen("/etc/serverview/sites-available/default", "w");
    if (config_file == NULL) {
        fprintf(stderr, BOLD_RED "Error creating config file: %s\n" COLOR_RESET, strerror(errno));
        return 1;
    }
    fprintf(config_file, "# ServerView CLI Config\n");
    fprintf(config_file, "port = 44080\n");
    fprintf(config_file, "base_path = /var/serverview/default\n");
    fprintf(config_file, "index_files = index.svh index.html index.htm\n");
    fclose(config_file);
    return 0;
}

int create_default_website() {
    printf(BOLD_CYAN "Creating default website...\n" COLOR_RESET);
    FILE *index_file = fopen("/var/serverview/default/index.svh", "w");
    if (index_file == NULL) {
        fprintf(stderr, BOLD_RED "Error creating default website: %s\n" COLOR_RESET, strerror(errno));
        return 1;
    }
    fprintf(index_file, 
        "<DOCTYPE html>\n"
        "<html>\n"
        "   <head>\n"
        "       <title>Server view default page</title>\n"
        "   </head>\n"
        "   <body>\n"
        "       <h1>Server view default page</h1>\n"
        "       <p>If you see this page, the server view CLI is successfully installed and working. The current version of the core is <system get=\"version\"></system></p>\n"
        "   </body>\n"
        "</html>\n"
    );
    fclose(index_file);
    return 0;
}

int setup() {
    printf(BOLD_CYAN "Setting up the environment...\n" COLOR_RESET);
    
    if (create_directories() != 0) {
        return 1;
    }
    if (create_default_config_file() != 0) {
        return 1;
    }
    if (create_default_website() != 0) {
        return 1;
    }

    return 0;
}
