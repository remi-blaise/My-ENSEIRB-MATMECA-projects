#include "server.h"

#define MAXCC 5   //maximal number of concurrent clients
#define POLL_INIT_SIZE 10
#define BUFF_SIZE 256


int main(int argc, char* argv[]){
    config_t config = get_config();
    logger_init(strcmp(argv[argc - 1], "-v") == 0);

    int sockfd, newsockfd, first_empty_fd, n;
    char buffer[BUFF_SIZE];
    char response[BUFF_SIZE];
    struct sockaddr_in serv_addr;
    struct pollfd* fd_array;

    int * pollsize = malloc(sizeof(int));   //TODO
    *pollsize = POLL_INIT_SIZE;
    fd_array = malloc(sizeof(struct pollfd) * (*pollsize));
    bzero(fd_array, sizeof(struct pollfd) * (*pollsize));

    for(int i = 0; i < *pollsize; i++) {
      fd_array[i].fd = -1; //poll function considers fd = -1 as empty
    }

    int port = argc >= 2 ? atoi(argv[1]) : config.controller_port;

    //========================================================================
    //setting up the server

    sockfd = do_socket();
    init_serv_addr(&serv_addr, port);
    do_bind(sockfd, &serv_addr);
    do_listen(sockfd, MAXCC);

    fd_array[0].fd = sockfd;
    fd_array[0].events = POLLIN;

    fd_array[1].fd = STDIN_FILENO;
    fd_array[1].events = POLLIN;

    first_empty_fd = 2;

    //========================================================================
    //setting up the database / init the broadcast thread

    init_aquarium(1000, 1000);
    broadcast_thread_init(fd_array);
    timeout_thread_init(fd_array, pollsize);


    //========================================================================
    //handling client's requests

    char log_buffer[BUFF_SIZE];
    sprintf(log_buffer, "Server up on port %d.", port);
    logger_log(log_buffer, ALWAYS);

    for(;;)
    {
        sprintf(log_buffer, "Server listening for requests...");
        logger_log(log_buffer, VERBOSE);

        printf(GRN "$ " RESET);
        fflush(stdout);

        poll(fd_array, *pollsize, -1); //waits for one of the file descriptors to become ready to perform I/O.

        //=============================
        //a client is trying to connect
        if (fd_array[0].revents & POLLIN) {
            sprintf(log_buffer, "A client is trying to connect.");
            logger_log(log_buffer, VERBOSE);

            newsockfd = do_accept(fd_array[0].fd);

            if(first_empty_fd >= *pollsize) {
                fd_array = realloc(fd_array, sizeof(struct pollfd) * (*pollsize) * 2);
                bzero(fd_array + *pollsize, sizeof(struct pollfd) * (*pollsize));

                for(int i = *pollsize; i < 2 * *pollsize; i++)
                    fd_array[i].fd = -1;

                *pollsize *= 2;
            }
            fd_array[first_empty_fd].fd = newsockfd;
            fd_array[first_empty_fd].events = POLLIN | POLLHUP;

            printf(BLEUE "    -> [client %d] connected\n\n" RESET, first_empty_fd);
            first_empty_fd++;

            sprintf(log_buffer, "[client %d] connected.", first_empty_fd);
            logger_log(log_buffer, ALWAYS);
        }

        //===================
        //a client is talking

        /* searching the socket that have sent the request */
        int ind_socket;
        for (ind_socket = 1; ind_socket < *pollsize; ind_socket++) {

            //if the peer closed it's end of the channel
            if (fd_array[ind_socket].revents & POLLHUP) {
                close_connection(ind_socket, fd_array, "bye");
                sprintf(log_buffer, "The peer has closed the connection %d.", ind_socket);
                logger_log(log_buffer, ALWAYS);
            }

            else if (fd_array[ind_socket].revents & POLLIN) {

                /* receiving the request from the socket */
                bzero(buffer, BUFF_SIZE);
                do_recv(ind_socket, fd_array, buffer);

                /* Handle command line */
                bzero((char *) response, BUFF_SIZE);
                if (ind_socket == 1) {
                    sprintf(log_buffer, "A message has been received from command line.");
                    logger_log(log_buffer, ALWAYS);

                    sprintf(log_buffer, "Message: %s.", buffer);
                    logger_log(log_buffer, VERBOSE);

                    command_execute(buffer, response);
                    do_send(ind_socket, fd_array, response);

                    sprintf(log_buffer, "A response has been sent in command line.");
                    logger_log(log_buffer, VERBOSE);
                }

                /* Handle client request */
                else {
                    sprintf(log_buffer, "A message has been received in connection %d.", ind_socket);
                    logger_log(log_buffer, ALWAYS);

                    sprintf(log_buffer, "Message: %s.", buffer);
                    logger_log(log_buffer, VERBOSE);

                    protocol_execute(buffer, response, fd_array, ind_socket);
                    do_send(ind_socket, fd_array, response);

                    sprintf(log_buffer, "A response has been sent in connection %d.", ind_socket);
                    logger_log(log_buffer, VERBOSE);
                }
            }
        }
    }

    free(fd_array);

    sprintf(log_buffer, "End of program.");
    logger_log(log_buffer, ALWAYS);

    return 0;
}


void error(char *msg){
    char log_buffer[BUFF_SIZE];
    sprintf(log_buffer, "An socket error has occured: %s", msg);
    logger_log(log_buffer, ALWAYS);

    perror(msg);
    exit(EXIT_FAILURE);
}


int do_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        error("ERROR openning socket");
    return sock;
}


void init_serv_addr(struct sockaddr_in * serv_addr, int portno) {
    bzero((char *) serv_addr, sizeof(struct sockaddr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    serv_addr->sin_port = htons(portno);
}


void do_bind(int sock, struct sockaddr_in * serv_addr) {
    if (bind(sock, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in)) < 0)
        error("ERROR on binding");
}


void do_listen(int sock, int max_cc){
    if (listen(sock, max_cc) < 0)
        error("ERROR on listen");
}


int do_accept(int sock) {
    int newsockfd = accept(sock, NULL, NULL);
    if (newsockfd < 0)
        error("ERROR on accept");
    return newsockfd;
}

void close_connection(int ind_socket, struct pollfd * fd_array, char * response) {
    free_view(fd_array[ind_socket].fd);
    close_socket(ind_socket);
    close(fd_array[ind_socket].fd);
    fd_array[ind_socket].fd = -1;
    printf(BLEUE "    -> [client %d] suddenly disconnected\n" RESET, ind_socket);
    fflush(stdout);
}

void do_send(int ind_socket, struct pollfd* fd_array, char * response) {
    int n = write(fd_array[ind_socket].fd, response, BUFF_SIZE);
    if (strcmp(response, "bye\n") == 0)
        close_connection(ind_socket, fd_array, response);
    if(n < 0)
        error("ERROR writing to socket");
    printf("\n");
}

void do_recv(int ind_socket, struct pollfd * fd_array, char * buffer) {
    int n = read(fd_array[ind_socket].fd, buffer, BUFF_SIZE - 1);
    if(n < 0)
        error("ERROR reading from socket");
}
