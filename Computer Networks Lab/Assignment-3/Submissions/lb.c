#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <time.h>
#include <poll.h>
#define bs 100
#define send_size 45
#define recs 45
#define POLL_SIZE 32

int recv_cli(char *buf, int newsockfd)
{
    char recv_buf[recs];
    for (int i = 0; i < recs; ++i)
        recv_buf[i] = '\0';
    int c = 0;
    int rs = recv(newsockfd, recv_buf, recs - 1, 0);
    if (rs <= 0)
        return rs;
    c += rs;
    for (int i = 0; i < bs; ++i)
        buf[i] = '\0';
    strcat(buf, recv_buf);
    while (recv_buf[rs - 1] != '\0')
    {
        rs = recv(newsockfd, recv_buf, recs - 1, 0);
        recv_buf[rs] = '\0';
        strcat(buf, recv_buf);
        c += rs;
    }
     //printf("AFTER ----------------------------------\n");
    return c;
}

// To send msg to client in chunks until it is completely sent
void send_cli(int sockfd, char *buf, int buf_len)
{
    char *start_ad = buf;
    int ss = 0;
    for (int i = 0; i < buf_len; ++i)
    {
        if ((i + 1) % send_size == 0)
        {
            send(sockfd, start_ad, send_size, 0);
            start_ad = buf + i + 1;
            ss = 0;
            continue;
        }
        ++ss;
        if (buf[i] == '\0')
        {
            send(sockfd, start_ad, ss, 0);
            break;
        }
    }
}
/* THE SERVER PROCESS */

void get_laods(struct sockaddr_in s1_addr, struct sockaddr_in s2_addr, int *s1, int *s2)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    if ((connect(sockfd, (struct sockaddr *)&s1_addr,
                 sizeof(s1_addr))) < 0)
    {
        perror("Unable to connect to server-1\n");
        exit(0);
    }
    char buf[bs];
    bzero(buf, bs);
    strcpy(buf, "Send Load");
    send(sockfd, buf, strlen(buf) + 1, 0);
    recv_cli(buf, sockfd);
    *s1 = atoi(buf);
    close(sockfd);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    if ((connect(sockfd, (struct sockaddr *)&s2_addr,
                 sizeof(s1_addr))) < 0)
    {
        perror("Unable to connect to server-2\n");
        exit(0);
    }
    bzero(buf, bs);
    strcpy(buf, "Send Load");
    send(sockfd, buf, strlen(buf) + 1, 0);
    recv_cli(buf, sockfd);
    *s2 = atoi(buf);
    close(sockfd);
    bzero(buf,bs);
    
    printf("Load received from %s is %d\n",inet_ntoa(s1_addr.sin_addr),*s1);
    printf("Load received from %s is %d\n",inet_ntoa(s2_addr.sin_addr),*s2);
    return;
}

int main(int argc,char *argv[])
{
    int s1_port, s2_port, lb_port, load_s1, load_s2;
    lb_port=atoi(argv[1]);
    s1_port=atoi(argv[2]);
    s2_port=atoi(argv[3]);
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr, lb_addr, s1_addr, s2_addr;
    char buf[bs];
    char recv_buf[45];
    struct pollfd set_poll[POLL_SIZE];
    int numfds = 0;
    int max_fd = 0;
    numfds++;
    int pol;
    for (int i = 0; i < bs; i++)
        buf[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // printf("Cannot create socket\n");
        exit(0);
    }
    lb_addr.sin_family = AF_INET;
    lb_addr.sin_addr.s_addr = INADDR_ANY;
    lb_addr.sin_port = htons(lb_port);

    s1_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &s1_addr.sin_addr);
    s1_addr.sin_port = htons(s1_port);

    s2_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &s2_addr.sin_addr);
    s2_addr.sin_port = htons(s2_port);

    if (bind(sockfd, (struct sockaddr *)&lb_addr,
             sizeof(lb_addr)) < 0)
    {
        // printf("Unable to bind local address\n");
        exit(0);
    }
    listen(sockfd, 5);
    time_t start_t, end_t;
    double diff_t;
    set_poll[0].fd = sockfd;
    set_poll[0].events = POLLIN;
    int init_time = 1;
    int timeout;
    while (1)
    {
        get_laods(s1_addr, s2_addr, &load_s1, &load_s2);
        init_time = 1;
        time(&start_t);
        while (1)
        {
            if (init_time == 1)
            {
                timeout = 5000;
            }
            else
            {
                timeout = 5000 - ((int)(difftime(end_t, start_t) * 1000));
                //printf("timeout is %d \n",timeout);
                if (timeout <= 0)
                    break;
            }
            pol = poll(set_poll, numfds, timeout);
            if (pol <= 0)
            {
                break;
            }
            if (set_poll[0].revents & POLLIN)
            {
                init_time = 0;
                clilen = sizeof(cli_addr);
                newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                                   &clilen);
                if (newsockfd < 0)
                {
                    // printf("Accept error\n");
                    exit(0);
                }

                if (fork() == 0)
                {
                    close(sockfd);
                    // client connected to newsockfd
                    // connect to server
                    if(load_s1<load_s2)
                    {
                        serv_addr=s1_addr;
                    }
                    else serv_addr=s2_addr;
                    int sockfd;
                    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                    {
                        perror("Cannot create socket\n");
                        exit(0);
                    }
                    if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                                 sizeof(serv_addr))) < 0)
                    {
                        perror("Unable to connect to server\n");
                        exit(0);
                    }
                    bzero(buf, bs);
                    strcpy(buf, "Send Time\0");
                    printf("Sending client request to %s\n",inet_ntoa(serv_addr.sin_addr));
                    send(sockfd, buf, strlen(buf) + 1, 0);
                    recv_cli(buf, sockfd);
                    close(sockfd);
                    send_cli(newsockfd,buf,strlen(buf)+1);
                    close(newsockfd);
                    exit(0);
                }
                close(newsockfd);
                time(&end_t);
                continue;
            }
        }
    }
    close(sockfd);
    return 0;
}
