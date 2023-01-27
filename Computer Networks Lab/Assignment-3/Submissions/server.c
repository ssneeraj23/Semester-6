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
#define recs 20
#define bs 30

int recv_cli(char *buf, int newsockfd)
{
    char recv_buf[recs];
    for (int i = 0; i < recs; ++i)
        recv_buf[i] = '\0';
    int c=0;
    int rs = recv(newsockfd, recv_buf, recs-1, 0);
    if (rs <= 0)
        return rs;
    c+=rs;
    for (int i = 0; i < bs; ++i)
        buf[i] = '\0';
    strcat(buf, recv_buf);
    while (recv_buf[rs - 1] != '\0')
    {
        rs = recv(newsockfd, recv_buf, recs-1, 0);
        recv_buf[rs]='\0';
        strcat(buf, recv_buf);
        c+=rs;
    }
    //printf("AFTER ----------------------------------\n");
    return c;
}

int main(int argc,char *argv[])
{
    int port=atoi(argv[1]);
	int sockfd,newsockfd,clilen;
    struct sockaddr_in	cli_addr, serv_addr;
    char mytime[64];
    time_t t ;
    char req[bs];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
    serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    srand (port%1001); 
    while(1)
    {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen) ;
    if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
    if(recv_cli(req,newsockfd)<=0)
    {
        close(newsockfd);
        continue;
    }
    if(strcmp(req,"Send Load\0")==0)
    {
        int load=(rand()%100)+1;
        bzero(req,bs);
        sprintf(req,"%d",load);
        printf("Load Sent : %d\n",load);
        send(newsockfd,req,strlen(req)+1,0);
        close(newsockfd);
        continue;
    }
    if(strcmp(req,"Send Time\0")==0)
    {
        bzero(mytime,64);
        t = time(NULL);
        struct tm *tm = localtime(&t);
        size_t ret = strftime(mytime, sizeof(mytime), "%c", tm);
    //printf("%s\n", mytime);
        send(newsockfd, mytime, strlen(mytime)+1, 0);
        close(newsockfd);
        continue;
    }
    close(newsockfd);
    }
    close(sockfd);
	return 0;
}