#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "process.h"
#include "config.h"

#define SVCORE_EXECUTABLE "/usr/local/sbin/svcore"

// Function to write a PID to a file
static int write_pid_file(const char *pid_file, pid_t pid) {
    FILE *file = fopen(pid_file, "w");
    if (!file) {
        perror("Could not open PID file for writing");
        return -1;
    }
    fprintf(file, "%d", pid);
    fclose(file);
    return 0;
}

// Function to read a PID from a file
static int read_pid_file(const char *pid_file) {
    FILE *file = fopen(pid_file, "r");
    if (!file) {
        // It's not an error if the file doesn't exist, it just means the process isn't running.
        if (errno == ENOENT) {
            return 0;
        }
        perror("Could not open PID file for reading");
        return -1;
    }
    int pid = 0;
    if (fscanf(file, "%d", &pid) != 1) {
        pid = 0;
    }
    fclose(file);
    return pid;
}

int start_process(const char *site_name, const char *config_path, const char *pid_file) {
    pid_t pid = read_pid_file(pid_file);
    if (pid > 0) {
        // Check if the process is actually running
        if (kill(pid, 0) == 0) {
            fprintf(stderr, "Process is already running with PID %d.\n", pid);
            return -1;
        }
    }

    SiteConfig *config = load_site_config(config_path);
    if (!config) {
        fprintf(stderr, "Failed to load site configuration.\n");
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        perror("Failed to fork");
        free_site_config(config);
        return -1;
    }

    if (pid == 0) { // Child process
        // Set environment variables
        if (config->port) {
            setenv("PORT", config->port, 1);
        }
        if (config->base_path) {
            setenv("BASE_PATH", config->base_path, 1);
        }
        if (config->num_index_files > 0) {
            size_t total_len = 0;
            for (int i = 0; i < config->num_index_files; i++) {
                total_len += strlen(config->index_files[i]);
            }
            total_len += config->num_index_files; // for commas and null terminator

            char *index_files_str = malloc(total_len);
            if (index_files_str) {
                index_files_str[0] = '\0';
                for (int i = 0; i < config->num_index_files; i++) {
                    strcat(index_files_str, config->index_files[i]);
                    if (i < config->num_index_files - 1) {
                        strcat(index_files_str, ",");
                    }
                }
                setenv("INDEX_FILES", index_files_str, 1);
                free(index_files_str);
            }
        }

        free_site_config(config); // Free config in child before exec

        // Redirect stdout and stderr to /dev/null
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null == -1) {
            perror("Failed to open /dev/null");
            exit(1);
        }
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null);

        execl(SVCORE_EXECUTABLE, SVCORE_EXECUTABLE, (char *)NULL);
        // execl only returns if there's an error
        perror("Failed to execute " SVCORE_EXECUTABLE);
        exit(1);
    }

    // Parent process
    free_site_config(config); // Free config in parent

    if (write_pid_file(pid_file, pid) != 0) {
        // Failed to write pid file, kill the child process
        kill(pid, SIGKILL);
        return -1;
    }

    printf("Site %s started with PID %d.\n", site_name, pid);
    return pid;
}

int stop_process(const char *site_name, const char *pid_file) {
    pid_t pid = read_pid_file(pid_file);
    if (pid <= 0) {
        fprintf(stderr, "Process is not running.\n");
        return -1;
    }

    if (kill(pid, SIGTERM) != 0) {
        perror("Failed to send SIGTERM to process");
        // If the process doesn't exist, remove the pid file
        if (errno == ESRCH) {
            unlink(pid_file);
        }
        return -1;
    }

    // Remove the pid file
    unlink(pid_file);

    printf("Site %s stopped.\n", site_name);
    return 0;
}

int get_process_status(const char *site_name, const char *pid_file) {
    pid_t pid = read_pid_file(pid_file);
    if (pid <= 0) {
        printf("Site %s is not running.\n", site_name);
        return 0;
    }

    if (kill(pid, 0) == 0) {
        printf("Site %s is running with PID %d.\n", site_name, pid);
        return pid;
    } else {
        if (errno == ESRCH) {
            printf("Site %s is not running (stale PID file).\n", site_name);
            // Clean up stale pid file
            unlink(pid_file);
        } else {
            perror("Could not determine process status");
        }
        return 0;
    }
}
