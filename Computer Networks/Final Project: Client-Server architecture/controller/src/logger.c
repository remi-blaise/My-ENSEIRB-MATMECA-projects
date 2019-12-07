#include "logger.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG_FILENAME "log"
#define BUFFER_SIZE 256

static int _verbosity;

void logger_init(int verbosity) {
    _verbosity = verbosity;
    logger_log("========================", ALWAYS);
    logger_log("Verbosity set to VERBOSE", VERBOSE);
}

void logger_log(char * message, int verbosity) {
    if (verbosity <= _verbosity) {
        FILE *fd = fopen(LOG_FILENAME, "a");
        if (fd == NULL) {
            printf(RED "  Error writing log file %s\n" RESET, LOG_FILENAME);
            exit(1);
        }

        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "[%02d/%02d/%d %02d:%02d:%02d] ", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        fwrite(buffer, 1, strlen(buffer), fd);

        fwrite(message, 1, strlen(message), fd);
        fwrite("\n", 1, 1, fd);

        fclose(fd);
    }
}
