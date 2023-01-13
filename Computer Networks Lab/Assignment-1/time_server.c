#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>


int main()
{
	int sockfd,newsockfd,clilen;
    struct sockaddr_in	cli_addr, serv_addr;
    char mytime[64];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
    serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(20000);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while(1)
    {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen) ;
    if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    size_t ret = strftime(mytime, sizeof(mytime), "%c", tm);
    //printf("%s\n", mytime);
    send(newsockfd, mytime, 64, 0);
    close(newsockfd);
    }
    close(sockfd);
	return 0;
}