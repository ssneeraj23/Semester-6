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
#define bs 64

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
	int sockfd,newsockfd,clilen,l_port;
    l_port=atoi(argv[1]);
    struct sockaddr_in serv_addr;
    char servtime[64];
    for(int i=0; i < 10; i++) servtime[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
    serv_addr.sin_family		= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(l_port);

    if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
    recv_cli(servtime,sockfd);
    printf("Well, the time on the server is \n");
    printf("%s",servtime);
    printf("\n");
    close(sockfd);
	return 0;
}