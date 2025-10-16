#ifndef DOWNLOAD_H
#define DOWNLOAD_H

char* get_latest_core_version();
char* get_latest_cli_version();
int download_file(const char *url, const char *output_path);

#endif