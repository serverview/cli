#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
    char *data;
    size_t size;
} Response;

int get_request(const char *url, Response *res);

#endif