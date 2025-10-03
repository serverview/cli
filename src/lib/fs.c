#include "fs.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h> // for strdup, free

// mkdir is in sys/stat.h

static int mkdir_p(const char *path) {
    char *p = strdup(path);
    if (p == NULL) return -1;
    char *sep = p;
    if (*sep == '/') sep++;
    while(1) {
        sep = strchr(sep, '/');
        if (sep == NULL) break;
        *sep = '\0';
        if (mkdir(p, 0755) && errno != EEXIST) {
            free(p);
            return -1;
        }
        *sep = '/';
        sep++;
    }
    if (mkdir(p, 0755) && errno != EEXIST) {
        free(p);
        return -1;
    }
    free(p);
    return 0;
}

int make_directory(const char *path) {
    return mkdir_p(path);
}
