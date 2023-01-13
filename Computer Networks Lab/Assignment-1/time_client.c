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
    struct sockaddr_in serv_addr;
    char servtime[64];
    for(int i=0; i < 10; i++) servtime[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
    serv_addr.sin_family		= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port		= htons(20000);

    if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
    recv(sockfd,servtime,64,0);
    printf("Well, the time on the server is \n");
    printf("%s",servtime);
    printf("\n");
    close(sockfd);
	return 0;
}