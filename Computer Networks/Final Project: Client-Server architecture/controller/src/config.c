#include "config.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_FILENAME "controller.cfg"
#define BUFFER_SIZE 200

config_t get_config() {
    FILE *fd = fopen(CONFIG_FILENAME, "r");
    if (fd == NULL) {
        printf(RED "  Can't read config file %s\n" RESET, CONFIG_FILENAME);
        exit(1);
    }

    char line[BUFFER_SIZE];
    int i = -1;
    config_t config = {0, 0, 0};

    while (fgets(line, BUFFER_SIZE, fd) != NULL) {
        ++i;
        if (line[0] == '#' || line[0] == '\n') continue;

        unsigned params_length = 2;
        char * params[params_length];
        char * regex = "^(controller-port|display-timeout-value|fish-update-interval) = ([[:digit:]]+)";

        if (regex_match(line, regex, params, params_length)) {
            if (strcmp(params[0], "controller-port") == 0) {
                config.controller_port = atoi(params[1]);
            } else if (strcmp(params[0], "display-timeout-value") == 0) {
                config.display_timeout_value = atoi(params[1]);
            } else if (strcmp(params[0], "fish-update-interval") == 0) {
                config.fish_update_interval = atoi(params[1]);
            } else {
                printf(RED "  Logic error.\n" RESET);
                exit(1);
            }

            free_params(params, params_length);
        } else {
            printf("%s\n", line);
            printf(RED "  Line %d is not valid in config file.\n" RESET, i);
            exit(1);
        }
    }

    if (!config.controller_port || !config.display_timeout_value || !config.fish_update_interval) {
        printf(RED "  Config file is not complete.\n" RESET);
        exit(1);
    }

    return config;
}
