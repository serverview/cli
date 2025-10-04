// Import standard libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "download.h"

// Import project's libs
#include "../lib/request.h"
#include "../lib/color.h"

int download_file(const char *url, const char *output_path) {
    char command[1024];
    // Use curl to download the file, -L follows redirects, -o specifies output file
    snprintf(command, sizeof(command), "curl -L -o %s %s", output_path, url);
    
    printf("Executing: %s\n", command); // For debugging
    
    int status = system(command);
    
    if (status == 0) {
        printf(BOLD_GREEN "Download successful.\n" COLOR_RESET);
    } else {
        fprintf(stderr, BOLD_RED "Download failed.\n" COLOR_RESET);
    }
    
    return status;
}

char* get_latest_core_version() {
    const char *url = "https://api.github.com/repos/serverview/core/releases/latest";
    Response res;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: svcli");

    if (get_request(url, &res, headers) != 0) {
        fprintf(stderr, "Failed to get latest version\n");
        curl_slist_free_all(headers);
        return NULL;
    }
    
    curl_slist_free_all(headers); // free the headers

    // Find the tag_name in the JSON response
    char *tag_name_key = "\"tag_name\":\"";
    char *start = strstr(res.data, tag_name_key);
    if (start == NULL) {
        fprintf(stderr, "Could not find tag_name in response\n");
        free(res.data);
        return NULL;
    }

    start += strlen(tag_name_key); // Move pointer to the start of the version string

    char *end = strchr(start, '"');
    if (end == NULL) {
        fprintf(stderr, "Could not find closing quote for tag_name\n");
        free(res.data);
        return NULL;
    }

    size_t version_len = end - start;
    char *version = malloc(version_len + 1);
    if (version == NULL) {
        fprintf(stderr, "Failed to allocate memory for version string\n");
        free(res.data);
        return NULL;
    }

    strncpy(version, start, version_len);
    version[version_len] = '\0';

    free(res.data); // Free the full response

    return version;
}