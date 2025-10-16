#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h> // Include json-c header
#include "download.h"

#include "../lib/request.h"
#include "../lib/color.h"

// Callback function for writing received data to a file
static size_t write_file_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int download_file(const char *url, const char *output_path) {
    CURL *curl;
    CURLcode res;
    FILE *fp;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(output_path, "wb");
        if (fp == NULL) {
            fprintf(stderr, BOLD_RED "Failed to open output file %s.\n" COLOR_RESET, output_path);
            curl_easy_cleanup(curl);
            return 1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects

        printf(BOLD_CYAN "Downloading %s to %s...\n" COLOR_RESET, url, output_path);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, BOLD_RED "Download failed: %s\n" COLOR_RESET, curl_easy_strerror(res));
            fclose(fp);
            curl_easy_cleanup(curl);
            return 1;
        }

        printf(BOLD_GREEN "Download successful.\n" COLOR_RESET);
        fclose(fp);
        curl_easy_cleanup(curl);
        return 0;
    }
    fprintf(stderr, BOLD_RED "Failed to initialize CURL for download.\n" COLOR_RESET);
    return 1;
}

static char* get_latest_version_from_github(const char *repo_name) {
    char url[256];
    snprintf(url, sizeof(url), "https://api.github.com/repos/serverview/%s/releases/latest", repo_name);

    Response res;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: serverview-controller-cli");

    if (get_request(url, &res, headers) != 0) {
        fprintf(stderr, "Failed to get latest version for %s\n", repo_name);
        curl_slist_free_all(headers);
        return NULL;
    }
    
    curl_slist_free_all(headers); // free the headers

    json_object *root = json_tokener_parse(res.data);
    free(res.data); // Free the raw response data after parsing

    if (root == NULL) {
        fprintf(stderr, "Failed to parse JSON response for %s version\n", repo_name);
        return NULL;
    }

    json_object *tag_name_obj;
    if (!json_object_object_get_ex(root, "tag_name", &tag_name_obj)) {
        fprintf(stderr, "Could not find 'tag_name' in JSON response for %s version\n", repo_name);
        json_object_put(root);
        return NULL;
    }

    const char *tag_name = json_object_get_string(tag_name_obj);
    char *version = strdup(tag_name);
    if (version == NULL) {
        perror("Failed to duplicate version string");
        json_object_put(root);
        return NULL;
    }

    json_object_put(root); // Free the JSON object

    return version;
}

char* get_latest_core_version() {
    return get_latest_version_from_github("core");
}

char* get_latest_cli_version() {
    return get_latest_version_from_github("cli");
}