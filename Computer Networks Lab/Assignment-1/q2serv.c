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
#define bs 25
int main()
{

    char new[]="none\n";

	int sockfd,newsockfd,clilen,rs;
    struct sockaddr_in	cli_addr, serv_addr;
    char exp[bs];
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
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
    for(int i=0;i<bs;++i)exp[i]='\0';
    // printf("before loop\n");
    while(1)
    {
        rs=recv(newsockfd,exp,bs,0);
        if(rs==0)break;
        if(exp[rs-1]=='\0')
        {
            
            printf("%s \n",exp);
            
            break;
        }
        for(int i=0;i<rs;++i)
        {
            printf("%c",exp[i]);
        }
    }
    // printf("out of loop\n");
    // printf("The new is %s\n",new);
    send(newsockfd,new,5,0);
    close(newsockfd);
    close(sockfd);
	return 0;
}