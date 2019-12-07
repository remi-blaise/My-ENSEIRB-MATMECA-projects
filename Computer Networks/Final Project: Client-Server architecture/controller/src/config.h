#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_s {
    int controller_port;
    int display_timeout_value;
    int fish_update_interval;
} config_t;

config_t get_config();

#endif
