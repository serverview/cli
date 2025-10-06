#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

#define VERSION_PATH_PREFIX "/etc/serverview/version/"

char* get_component_version(const char *component) {
    char path[256];
    snprintf(path, sizeof(path), "%s%s", VERSION_PATH_PREFIX, component);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return NULL;
    }

    char *version = NULL;
    size_t len = 0;
    if (getline(&version, &len, file) == -1) {
        free(version);
        fclose(file);
        return NULL;
    }

    // Remove newline character if present
    version[strcspn(version, "\n")] = 0;

    fclose(file);
    return version;
}

int save_component_version(const char *component, const char *version) {
    char path[256];
    snprintf(path, sizeof(path), "%s%s", VERSION_PATH_PREFIX, component);

    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("Failed to open version file for writing");
        return 1;
    }

    if (fprintf(file, "%s\n", version) < 0) {
        perror("Failed to write version to file");
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}
