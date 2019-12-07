#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define BUFF_SIZE 256


#define YEL "\x1B[0;33m"
#define GRN   "\x1B[92m"
#define RESET "\x1B[0m"


void error(char* msg){
  perror(msg);
  exit(0);
}

int main(int argc, char* argv[]){
    int sockfd, portno, n, newsockfd;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[BUFF_SIZE];
    char response[BUFF_SIZE];

    if(argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd < 0) error("ERROR creating socket");

    server = gethostbyname(argv[1]);
    if(server==NULL){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*)&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);


    //=============================
    // POLLFD Initialisation
    struct pollfd* fd_array;
    int poll_size = 2;
    fd_array = malloc(sizeof(struct pollfd) * poll_size);
    bzero(fd_array, sizeof(struct pollfd) * poll_size);


    fd_array[0].fd = STDIN_FILENO;
    fd_array[0].events = POLLIN;

    if(newsockfd = connect(sockfd, (struct sockaddr*)&serv_addr ,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    fd_array[1].fd = newsockfd;
    fd_array[1].events = POLLIN | POLLHUP;

    //=============================
    // Listening to the sockets
    for (;;)
    {
        printf(GRN "$ " RESET);
        fflush(stdout);

        poll(fd_array, poll_size, -1); //waits for one of a set of file descriptors to become ready to perform I/O.

        if (fd_array[0].revents & POLLIN)
        {
            bzero(buffer,256);
            n = read(fd_array[0].fd, buffer, BUFF_SIZE - 1);
            if(n < 0) error("ERROR reading from stdin");

            n = write(fd_array[1].fd, buffer, strlen(buffer) + 1);
            if(n < 0) error("ERROR writing to socket");

            bzero(buffer,256);
            n = read(fd_array[1].fd, buffer, sizeof(buffer));
            if(n < 0) error("ERROR reading from socket");

            printf( YEL "> " RESET);
            printf("%s\n\n",buffer);
        }
        else
        {
            bzero(buffer,256);
            n = read(fd_array[1].fd, buffer, sizeof(buffer));
            if(n < 0) error("ERROR reading from socket");

            printf( YEL "> " RESET);
            printf("%s\n\n",buffer);
        }
    }


    return 0;
}
