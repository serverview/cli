#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config.h"

// Helper function to trim leading and trailing whitespace
char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

SiteConfig* load_site_config(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Could not open config file");
        return NULL;
    }

    SiteConfig *config = malloc(sizeof(SiteConfig));
    if (!config) {
        perror("Could not allocate memory for config");
        fclose(file);
        return NULL;
    }
    // Initialize config
    config->port = NULL;
    config->base_path = NULL;
    config->index_files = NULL;
    config->num_index_files = 0;

    char *line = NULL;
    size_t len = 0;
    size_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            key = trim_whitespace(key);
            value = trim_whitespace(value);

            if (strcmp(key, "port") == 0) {
                config->port = strdup(value);
            } else if (strcmp(key, "base_path") == 0) {
                config->base_path = strdup(value);
            } else if (strcmp(key, "index_files") == 0) {
                char *token = strtok(value, " ");
                while (token) {
                    config->index_files = realloc(config->index_files, sizeof(char*) * (config->num_index_files + 1));
                    config->index_files[config->num_index_files] = strdup(token);
                    config->num_index_files++;
                    token = strtok(NULL, " ");
                }
            }
        }
    }

    fclose(file);
    if (line) {
        free(line);
    }

    return config;
}

void free_site_config(SiteConfig *config) {
    if (config) {
        if (config->port) free(config->port);
        if (config->base_path) free(config->base_path);
        if (config->index_files) {
            for (int i = 0; i < config->num_index_files; i++) {
                free(config->index_files[i]);
            }
            free(config->index_files);
        }
        free(config);
    }
}

ServerConfig* load_server_config(const char *sites_enabled_dir) {
    // TODO: Implement this function
    // It should scan the directory for config files,
    // load each one using load_site_config,
    // and add it to a ServerConfig struct.
    return NULL;
}

void free_server_config(ServerConfig *config) {
    if (config) {
        if (config->sites) {
            for (int i = 0; i < config->num_sites; i++) {
                free_site_config(config->sites[i]);
            }
            free(config->sites);
        }
        free(config);
    }
}
