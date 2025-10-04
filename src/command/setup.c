// Import standard libs
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

// Import project's libs
#include "../lib/color.h"
#include "../lib/fs.h"
#include "../lib/download.h"
#include "../lib/process.h"

const char *core_download_path = "/tmp/svcore";

int download_core(const char *version_tag) { // version_tag is for example "v0.0.3"
    printf(BOLD_CYAN "Downloading core version %s...\n" COLOR_RESET, version_tag);

    // Get version without 'v' prefix
    const char *version_number = version_tag;
    if (version_number[0] == 'v') {
        version_number++;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "svcore_%s_linux_amd64", version_number);

    char url[256];
    snprintf(url, sizeof(url), "https://github.com/serverview/core/releases/download/%s/%s", version_tag, filename);

    if (download_file(url, core_download_path) != 0) {
        fprintf(stderr, BOLD_RED "Failed to download core.\n" COLOR_RESET);
        return 1;
    }

    return 0;
}

int install_core() {
    printf(BOLD_CYAN "Installing core...\n" COLOR_RESET);

    const char *install_path = "/usr/local/sbin/svcore";

    // Move the file
    if (rename(core_download_path, install_path) != 0) {
        perror(BOLD_RED "Failed to move core to installation directory" COLOR_RESET);
        return 1;
    }

    // Make it executable
    if (chmod(install_path, 0755) != 0) {
        perror(BOLD_RED "Failed to make core executable" COLOR_RESET);
        return 1;
    }

    printf(BOLD_GREEN "Core installed successfully.\n" COLOR_RESET);
    return 0;
}

int create_directories() {
    printf(BOLD_CYAN "Creating necessary directories...\n" COLOR_RESET);

    const char *directories[] = {
        "/var/www/svh",
        "/etc/serverview",
        "/etc/serverview/sites-enabled",
        "/etc/serverview/sites-available",
        "/var/log/serverview",
        "/var/serverview/default",
        "/var/run/serverview"
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
        "       <p>If you see this page, the server view CLI is successfully installed and working. The current version of the core is <system get=\"version\"></system>.</p>\n"
        "   </body>\n"
        "</html>\n"
    );
    fclose(index_file);
    return 0;
}

int enable_default_site() {
    printf(BOLD_CYAN "Enabling default site...\n" COLOR_RESET);

    const char *target = "/etc/serverview/sites-available/default";
    const char *linkpath = "/etc/serverview/sites-enabled/default";

    if (symlink(target, linkpath) != 0) {
        perror(BOLD_RED "Failed to enable default site" COLOR_RESET);
        return 1;
    }

    printf(BOLD_GREEN "Default site enabled successfully.\n" COLOR_RESET);
    return 0;
}

int install_service(const char *executable_path) {
    printf(BOLD_CYAN "Installing systemd service...\n" COLOR_RESET);

    const char *service_file_path = "/etc/systemd/system/svcore.service";
    FILE *service_file = fopen(service_file_path, "w");
    if (!service_file) {
        perror("Failed to create service file");
        return 1;
    }

    fprintf(service_file, "[Unit]\n");
    fprintf(service_file, "Description=svcore service\n");
    fprintf(service_file, "After=network.target\n\n");
    fprintf(service_file, "[Service]\n");
    fprintf(service_file, "Type=forking\n");
    fprintf(service_file, "ExecStart=%s start-all\n", executable_path);
    fprintf(service_file, "Restart=on-failure\n");
    fprintf(service_file, "User=root\n\n");
    fprintf(service_file, "[Install]\n");
    fprintf(service_file, "WantedBy=multi-user.target\n");

    fclose(service_file);

    printf(BOLD_CYAN "Reloading systemd daemon...\n" COLOR_RESET);
    if (system("systemctl daemon-reload") != 0) {
        fprintf(stderr, "Failed to reload systemd daemon.\n");
        // This is not a fatal error, so we don't return 1
    }

    printf(BOLD_CYAN "Enabling svcore service...\n" COLOR_RESET);
    if (system("systemctl enable svcore") != 0) {
        fprintf(stderr, "Failed to enable svcore service.\n");
        // This is not a fatal error
    }

    printf(BOLD_GREEN "Systemd service installed and enabled successfully.\n" COLOR_RESET);
    return 0;
}

int start_service() {
    printf(BOLD_CYAN "Starting svcore service...\n" COLOR_RESET);
    if (system("systemctl start svcore") != 0) {
        fprintf(stderr, BOLD_RED "Failed to start svcore service.\n" COLOR_RESET);
        return 1;
    }
    printf(BOLD_GREEN "svcore service started successfully.\n" COLOR_RESET);
}

int setup(const char *executable_path) {
    if (geteuid() != 0) {
        fprintf(stderr, BOLD_RED "This command must be run as root.\n" COLOR_RESET);
        return 1;
    }

    printf(BOLD_CYAN "Setting up the environment...\n" COLOR_RESET);

    char *latest_version = get_latest_core_version();
    if (latest_version == NULL) {
        fprintf(stderr, "Failed to get the latest core version.\n");
        return 1; // Exit if we can't get the version
    }
    
    printf("Latest core version: %s\n", latest_version);

    if (download_core(latest_version) != 0) {
        free(latest_version);
        return 1;
    }

    free(latest_version); // free the version string as soon as we are done with it

    if (install_core() != 0) {
        return 1;
    }
    
    if (create_directories() != 0) {
        return 1;
    }
    if (create_default_config_file() != 0) {
        return 1;
    }
    if (create_default_website() != 0) {
        return 1;
    }
    if (enable_default_site() != 0) {
        return 1;
    }
    if (install_service(executable_path) != 0) {
        return 1;
    }
    if (start_service() != 0) {
        return 1;
    }

    return 0;
}
