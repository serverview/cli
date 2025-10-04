#ifndef REQUEST_H
#define REQUEST_H

#include <curl/curl.h>

typedef struct {
    char *data;
    size_t size;
} Response;

int get_request(const char *url, Response *res, struct curl_slist *headers);

#endif