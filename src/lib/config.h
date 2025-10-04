#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char *port;
    char *base_path;
    char **index_files;
    int num_index_files;
} SiteConfig;

typedef struct {
    SiteConfig **sites; // Array of pointers to SiteConfig
    int num_sites;
} ServerConfig;

SiteConfig* load_site_config(const char *filepath);
void free_site_config(SiteConfig *config);

ServerConfig* load_server_config(const char *sites_enabled_dir);
void free_server_config(ServerConfig *config);

#endif