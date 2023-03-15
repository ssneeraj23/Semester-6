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
    // send_table_len=0; recv_table_len=0;
    // send_table_index=0;recv_table_index=0;
    int sockfd, newsockfd, clilen;
    struct sockaddr_in cli_addr, serv_addr;
    char mytime[100];
    bzero(mytime, 100);
    if ((sockfd = my_socket(AF_INET, SOCK_MyTCP, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));
    if (my_bind(sockfd, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Unable to bind local address\n");
        exit(0);
    }
    my_listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    printf("connected\n");
    while (1)
    {
        newsockfd = my_accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        printf("my sockfd_new one is %d\n", newsockfd);
        if (newsockfd < 0)
        {
            perror("Accept error\n");
            exit(0);
        }
        strcpy(mytime, "Lorem ipsum dolor sit amet, consectetur a");
        sleep(2);
        int rs = my_send(newsockfd, mytime, strlen(mytime), 0);
        printf("message sent from server is %s and length is %d\n", mytime, rs);
        strcpy(mytime, "-------123456789-------");
        sleep(2);
        rs = my_send(newsockfd, mytime, strlen(mytime), 0);
        printf("message sent from server is %s and length is %d\n", mytime, rs);
        sleep(100);
    }
    close(sockfd);
    return 0;
}
