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


#define YEL "\x1B[0;33m"
#define GRN   "\x1B[92m"
#define RESET "\x1B[0m"


void error(char* msg){
  perror(msg);
  exit(0);
}

int main(int argc, char* argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[256];
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

    // printf("%s\n", inet_ntoa(serv_addr.sin_addr));
    // printf("%d\n",portno );

    if(connect(sockfd, (struct sockaddr*)&serv_addr ,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // sleep(0.5);
    // bzero(buffer,256);
    // n = read(sockfd,buffer,sizeof(buffer));
    // if(n<0) error("ERROR reading from socket");
    // printf("%s\n\n",buffer);

    do {
        printf(GRN "$ " RESET);

        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd, buffer, strlen(buffer) + 1);
        if(n < 0) error("ERROR writing to socket");

        bzero(buffer,256);
        n = read(sockfd,buffer,sizeof(buffer));
        if(n < 0) error("ERROR reading from socket");
        printf( YEL "> " RESET);
        printf("%s\n\n",buffer);

        //close(sockfd);
        //sockfd = socket(AF_INET, SOCK_STREAM,0);
    } while(1);

    return 0;
}
