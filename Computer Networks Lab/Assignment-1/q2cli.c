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
#define bs 20

int main()
{
    int done=0;
    int first_pack;
    int term=0;
    int sockfd,sendsize;
    struct sockaddr_in serv_addr;
    char exp[bs];
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
    while(1)
    {
        
        first_pack=0;
        printf("Enter the expression to be evaluated by the mighty server\n");
    while(1)
    {
        for(int i=0;i<bs;++i)exp[i]='0';
        fgets(exp,bs,stdin);
        sendsize=-1;
        for(int i=0;i<bs;++i)
        {
            if(exp[i]=='\n')
            {
                sendsize=i+1;
                done=1;
                exp[i]='\0';
                break;
            }
        }
        if(exp[0]=='-'&&exp[1]=='1'&&exp[2]=='\0'&&first_pack==0)
        {
            printf("Oh ok, you wish to turn me off\nGood bye!\n");
            term=1;
            break;
        }
        first_pack=1;
        if(sendsize==-1)
        {
            sendsize=19;
        }
        send(sockfd,exp,sendsize,0);
        if(done)break;
    }
    if(term)break;
    for(int i=0;i<bs;++i)exp[i]='\0';
    int rs=recv(sockfd,exp,bs,0);
    printf("Well the answer (as told by the ""Mighty Server"") is \n");
    printf("%s",exp);
    printf("\n");
    }
    close(sockfd);
	return 0;
}