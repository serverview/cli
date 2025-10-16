#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>

#include "../lib/color.h"
#include "../lib/download.h"
#include "../lib/version.h"
#include "../cli_version.h" // Include CLI version header
#include "start-all.h"
#include "stop-all.h"

// Forward declarations for functions defined in setup.c
extern int download_core(const char *version_tag);
extern int install_core();

// Function to update the CLI itself
int update_cli(const char *current_cli_path) {
    printf(BOLD_CYAN "Checking for CLI updates...\n" COLOR_RESET);

    char *latest_cli_version = get_latest_cli_version();
    if (latest_cli_version == NULL) {
        fprintf(stderr, BOLD_RED "Failed to get the latest CLI version.\n" COLOR_RESET);
        return 1;
    }

    const char *current_cli_version = CLI_VERSION; // Use the defined macro

    printf("Current CLI version: %s, Latest CLI version: %s\n", current_cli_version, latest_cli_version);

    if (strcmp(current_cli_version, latest_cli_version) == 0) {
        printf(BOLD_GREEN "You are already on the latest version of the CLI.\n" COLOR_RESET);
        free(latest_cli_version);
        return 0;
    }

    printf(BOLD_CYAN "Upgrading CLI to %s...\n" COLOR_RESET, latest_cli_version);

    char temp_cli_path[PATH_MAX];
    snprintf(temp_cli_path, sizeof(temp_cli_path), "/tmp/svcli_new");

    char cli_filename[128];
    // Assuming the filename format is svctl_X.Y.Z_linux_amd64
    char *version_number = latest_cli_version;
    if (version_number[0] == 'v') {
        version_number++;
    }
    snprintf(cli_filename, sizeof(cli_filename), "svctl_%s_linux_amd64", version_number);

    char cli_download_url[256];
    snprintf(cli_download_url, sizeof(cli_download_url), "https://github.com/serverview/cli/releases/download/%s/%s", latest_cli_version, cli_filename);

    if (download_file(cli_download_url, temp_cli_path) != 0) {
        fprintf(stderr, BOLD_RED "Failed to download new CLI version.\n" COLOR_RESET);
        free(latest_cli_version);
        return 1;
    }

    // Make the downloaded file executable
    if (chmod(temp_cli_path, 0755) != 0) {
        perror(BOLD_RED "Failed to make new CLI executable" COLOR_RESET);
        free(latest_cli_version);
        return 1;
    }

    // Create a self-replacement script
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), "/tmp/update_svcli.sh");

    FILE *script_file = fopen(script_path, "w");
    if (script_file == NULL) {
        perror(BOLD_RED "Failed to create self-update script" COLOR_RESET);
        free(latest_cli_version);
        return 1;
    }

    fprintf(script_file, "#!/bin/bash\n");
    fprintf(script_file, "# Wait for the current CLI process to exit\n");
    fprintf(script_file, "sleep 1\n");
    fprintf(script_file, "# Replace the old CLI with the new one\n");
    fprintf(script_file, "mv %s %s\n", temp_cli_path, current_cli_path);
    fprintf(script_file, "# Make sure it's executable\n");
    fprintf(script_file, "chmod 0755 %s\n", current_cli_path);
    fprintf(script_file, "echo \"" BOLD_GREEN "CLI updated successfully to %s.\"" COLOR_RESET "\n", latest_cli_version);
    fprintf(script_file, "# Clean up\n");
    fprintf(script_file, "rm -- \"%s\"\n", script_path);
    fprintf(script_file, "# Restart the CLI (optional, depending on desired behavior)\n");
    // For now, we won't restart automatically. The user will re-run the command.

    fclose(script_file);

    // Make the script executable
    if (chmod(script_path, 0755) != 0) {
        perror(BOLD_RED "Failed to make self-update script executable" COLOR_RESET);
        free(latest_cli_version);
        return 1;
    }

    printf(BOLD_CYAN "CLI update prepared. Please re-run the command after this process exits.\n" COLOR_RESET);

    // Execute the script and exit the current process
    execl(script_path, script_path, (char *)NULL);
    perror(BOLD_RED "Failed to execute self-update script" COLOR_RESET);
    free(latest_cli_version);
    return 1; // Should not be reached
}

int upgrade(const char *executable_path) {
    if (geteuid() != 0) {
        fprintf(stderr, BOLD_RED "This command must be run as root.\n" COLOR_RESET);
        return 1;
    }

    // First, check and update the CLI itself
    if (update_cli(executable_path) != 0) {
        fprintf(stderr, BOLD_RED "CLI update failed. Aborting core upgrade.\n" COLOR_RESET);
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

    stop_all();

    if (download_core(latest_version) != 0) {
        fprintf(stderr, BOLD_RED "Failed to download new version.\n" COLOR_RESET);
        start_all(); // Attempt to restart the service
        free(installed_version);
        free(latest_version);
        return 1;
    }

    if (install_core() != 0) {
        fprintf(stderr, BOLD_RED "Failed to install new version.\n" COLOR_RESET);
        start_all(); // Attempt to restart the service
        free(installed_version);
        free(latest_version);
        return 1;
    }

    if (save_component_version("core", latest_version) != 0) {
        fprintf(stderr, BOLD_YELLOW "Warning: Could not save new core version.\n" COLOR_RESET);
    }

    printf(BOLD_GREEN "Core upgraded successfully to %s.\n" COLOR_RESET, latest_version);

    start_all();

    free(installed_version);
    free(latest_version);

    return 0;
}