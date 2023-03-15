#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <stdbool.h>
#include "mysocket.h"

int main(int argc, char **argv)
{
    printf("In main\n");
    int sockfd;
    struct sockaddr_in serv_addr;

    int i;
    char buf[100];

    sockfd = my_socket(AF_INET, SOCK_MyTCP, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("socket created\n");
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if ((my_connect(sockfd, (struct sockaddr *)&serv_addr,
                    sizeof(serv_addr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }
    printf("connected...\n");
    char buff[100];
    bzero(buff, 100);
    int rs = my_recv(sockfd, buff, 100, 0);
    printf("received msg in client is %s and length is %d\n", buff, rs);
    bzero(buff, 100);
    rs = my_recv(sockfd, buff, 100, 0);
    printf("second msg received is ---- %s and length is %d\n", buff, rs);
    return 0;
}
