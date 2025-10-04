#ifndef PROCESS_H
#define PROCESS_H

int start_process(const char *config_path, const char *pid_file);
int stop_process(const char *pid_file);
int get_process_status(const char *pid_file);

#endif