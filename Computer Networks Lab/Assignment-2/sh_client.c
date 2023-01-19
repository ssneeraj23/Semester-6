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
#define bs 200
#define recs 45

void recv_serv(char *buf, int newsockfd)
{
    char recv_buf[recs];
    for (int i = 0; i < recs; ++i)
        recv_buf[i] = '\0';
    int rs = recv(newsockfd, recv_buf, recs, 0);
    if (rs <= 0)
        return;
    for (int i = 0; i < bs; ++i)
        buf[i] = '\0';
    strcat(buf, recv_buf);
    while (recv_buf[rs - 1] != '\0')
    {
        rs = recv(newsockfd, recv_buf, recs, 0);
        strcat(buf, recv_buf);
    }
    // printf("AFTER ----------------------------------\n");
    return;
}
int main()
{
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr;
    char buf[bs];
    for (int i = 0; i < bs; i++)
        buf[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(20000);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                 sizeof(serv_addr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }
    recv(sockfd, buf, 10, 0);
    printf("%s", buf);
    printf("\n"); // prints login
    for (int i = 0; i < bs; i++)
        buf[i] = '\0';
    fgets(buf, 26, stdin);
    int ss = 0;
    for (; ss < 26; ++ss)
    {
        if (buf[ss] == '\n')
        {
            buf[ss] = '\0';
            ++ss;
            break;
        }
    }
    send(sockfd, buf, ss, 0);
    recv(sockfd, buf, 10, 0);
    if (buf[0] == 'N')
    {
        printf("Invalid Username\n");
        close(sockfd);
        exit(0);
    }
    char send_buf[50];
    while (1)
    {
        printf("Please enter a command to be run in the server\n");
        fgets(buf, bs, stdin);
        if(strcmp(buf,"exit\n")==0)break;
        char *start_ad = buf;
        ss = 0;
        for (int i = 0; i < bs; ++i)
        {
            if ((i + 1) % 45  == 0)
            {
                send(sockfd, start_ad, 45, 0);
                start_ad = buf + i + 1;
                ss = 0;
                continue;
            }
            ++ss;
            if (buf[i] == '\n')
            {
                buf[i] = '\0';
                send(sockfd, start_ad, ss, 0);
                break;
            }
        }
        bzero(buf,bs);
        recv_serv(buf,sockfd);
        if(buf[0]=='\0')
        {
           continue;
        }
        else
        {
            if(strcmp(buf,"$$$$")==0)
            {
                printf("Invalid Command\n");
            }
            else
            {
                  if(strcmp(buf,"####")==0)
                  {
                    printf("Error in running command\n");
                  }
                  else
                  printf("%s\n",buf);
            }
        }

    }
    close(sockfd);
    return 0;
}