#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "process.h"

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

int start_process(const char *config_path, const char *pid_file) {
    pid_t pid = read_pid_file(pid_file);
    if (pid > 0) {
        // Check if the process is actually running
        if (kill(pid, 0) == 0) {
            fprintf(stderr, "Process is already running with PID %d.\n", pid);
            return -1;
        }
    }

    pid = fork();
    if (pid < 0) {
        perror("Failed to fork");
        return -1;
    }

    if (pid == 0) { // Child process
        execl(SVCORE_EXECUTABLE, SVCORE_EXECUTABLE, "--config", config_path, (char *)NULL);
        // execl only returns if there's an error
        perror("Failed to execute " SVCORE_EXECUTABLE);
        exit(1);
    }

    // Parent process
    if (write_pid_file(pid_file, pid) != 0) {
        // Failed to write pid file, kill the child process
        kill(pid, SIGKILL);
        return -1;
    }

    printf("Process started with PID %d.\n", pid);
    return pid;
}

int stop_process(const char *pid_file) {
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

    printf("Process with PID %d stopped.\n", pid);
    return 0;
}

int get_process_status(const char *pid_file) {
    pid_t pid = read_pid_file(pid_file);
    if (pid <= 0) {
        printf("Process is not running.\n");
        return 0;
    }

    if (kill(pid, 0) == 0) {
        printf("Process is running with PID %d.\n", pid);
        return pid;
    } else {
        if (errno == ESRCH) {
            printf("Process with PID %d is not running (stale PID file).\n", pid);
            // Clean up stale pid file
            unlink(pid_file);
        } else {
            perror("Could not determine process status");
        }
        return 0;
    }
}
