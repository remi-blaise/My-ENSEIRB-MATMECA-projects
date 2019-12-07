#ifndef LOGGER_H
#define LOGGER_H

#define ALWAYS 0
#define VERBOSE 1

void logger_init(int verbosity);
void logger_log(char * message, int verbosity);

#endif
