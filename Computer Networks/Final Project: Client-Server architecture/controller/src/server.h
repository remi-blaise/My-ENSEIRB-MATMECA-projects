#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <poll.h>
#include <string.h>

#include "command.h"
#include "protocol.h"
#include "state.h"
#include "utils.h"
#include "config.h"
#include "logger.h"

void error(char *msg);
void init_serv_addr(struct sockaddr_in * serv_addr, int portno);

int do_socket();
void do_bind(int sock, struct sockaddr_in * serv_addr);
void do_listen(int sock, int max_cc);
int do_accept(int sock);

void close_connection(int ind_socket, struct pollfd * fd_array, char * response);

void do_send(int ind_socket, struct pollfd* fd_array, char * response);
void do_recv(int ind_socket, struct pollfd * fd_array, char * buffer);


#endif
