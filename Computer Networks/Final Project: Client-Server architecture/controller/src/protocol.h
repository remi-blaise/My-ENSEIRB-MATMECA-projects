#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <poll.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>
#include "utils.h"
#include "errno.h"
#include <pthread.h>
#include <errno.h>
#include <malloc.h>
#include "config.h"

#include "state.h"
#include <sys/mman.h>

#define BUFF_SIZE 256

void protocol_execute(char * message, char * response, struct pollfd* fd_array, int ind_socket); //switch function

void broadcast_thread_init(struct pollfd * fd_array);
void timeout_thread_init(struct pollfd * fd_array, int * pollsize);

void close_socket(int ind_socket);

#endif
