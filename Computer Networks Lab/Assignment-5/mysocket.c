#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <stdbool.h>

#define SOCK_MyTCP 18

typedef struct
{
     int sock;
} MyTCP;

MyTCP curr_socket;
MyTCP main_socket;
char **send_message;
char **received_message;
int send_table_len, recv_table_len;
int send_table_index,recv_table_index;
pthread_t Recvt, Sendt;
pthread_cond_t cond_recv = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_send = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_send_full = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_recv = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_send = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_dummy1 = PTHREAD_MUTEX_INITIALIZER;

MyTCP my_socket(int domain, int type, int protocol);
int my_bind(MyTCP mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_listen(MyTCP mysockfd, int backlog);
MyTCP my_accept(MyTCP mysockfd, struct sockaddr *addr, socklen_t *addrlen);
int my_connect(MyTCP mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_send(MyTCP mysockfd, const void *buf, size_t len, int flags);
int my_recv(MyTCP mysockfd, void *buf, size_t len, int flags);
int my_close(MyTCP mysockfd);

void *R(void *arg)
{
     while (1)
     {
          pthread_lock(&mutex_recv);
          pthread_cond_wait(&cond_recv, &mutex_recv);
     }
     return NULL;
}

void *S(void *arg)
{
     while (1)
     {
          pthread_lock(&mutex_send);
          pthread_cond_wait(&cond_send, &mutex_send);
     }
     return NULL;
}

int main()
{

     return 0;
}

MyTCP my_socket(int domain, int type, int protocol)
{

     if (type != SOCK_MyTCP)
     {
          MyTCP mytcp;
          mytcp.sock = -1;
          return mytcp;
     }
     else
     {

          MyTCP mytcp;
          mytcp.sock = socket(domain, SOCK_STREAM, protocol);
          if (mytcp.sock < 0)
          {
               mytcp.sock = -1;
               return mytcp;
          }
          // allocate space for the tables
          send_message = (char **)malloc(10 * sizeof(char *));
          for (int i = 0; i < 10; i++)
          {
               send_message[i] = (char *)malloc(1000 * sizeof(char));
          }
          received_message = (char **)malloc(10 * sizeof(char *));
          for (int i = 0; i < 10; i++)
          {
               received_message[i] = (char *)malloc(1000 * sizeof(char));
          }
          // create threads and initialize them

          pthread_attr_t attr;
          pthread_attr_init(&attr);
          curr_socket = mytcp;
          main_socket = mytcp;
          pthread_create(&Sendt, &attr, R, NULL);
          pthread_create(&Recvt, &attr, S, NULL);
          return mytcp;
     }
}

int my_bind(MyTCP mysockfd, const struct sockaddr *addr, socklen_t addrlen)
{
     return bind(mysockfd.sock, addr, addrlen);
}

int my_listen(MyTCP mysockfd, int backlog)
{
     int ret = listen(mysockfd.sock, backlog);
     if (ret < 0)
     {
          perror("Error in listen");
          return -1;
     }
     return ret;
}

MyTCP my_accept(MyTCP mysockfd, struct sockaddr *cli_addr, socklen_t *clilen)
{
     MyTCP newsocktcp;
     newsockfd = accept(mysockfd.sock, (struct sockaddr *)&cli_addr, &clilen);
     if (newsockfd < 0)
     {
          newsocktcp.sock = -1;
          return newsocktcp;
     }
     newsocktcp = curr_socket;
     newsocktcp.sock = newsockfd;
     curr_socket.sock = newsockfd;
     // broadcast the signal
     return newsocktcp;
}

int my_connect(MyTCP mysockfd, const struct sockaddr *addr, socklen_t addrlen)
{

     int res = connect(mysockfd.sock, addr, addrlen);
     if (res == -1)
     {
          perror("Connection failed");
          return -1;
     }
     return 0;
}

int my_close(MyTCP mysockfd)
{
     if (mysockfd.sock == main_socket.sock)
     {
          pthread_mutex_destroy(&mutex_send);
          pthread_mutex_destroy(&mutex_recv);
          pthread_cond_destroy(&cond_send);
          pthread_cond_destroy(&cond_recv);
          pthread_exit(&Sendt);
          pthread_exit(&Recvt);
          if (send_message != NULL)
          {
               for (int i = 0; i < 10; i++)
               {
                    if (send_message[i] != NULL)
                    {
                         free(send_message[i]);
                    }
               }
               free(send_message);
          }

          // Free received_message
          if (received_message != NULL)
          {
               for (int i = 0; i < 10; i++)
               {
                    if (received_message[i] != NULL)
                    {
                         free(received_message[i]);
                    }
               }
               free(received_message);
          }

          close(mysockfd.sock);
     }
     else {
          //tharavatha rastha
     }
}

int my_send(MyTCP mysockfd, const void *buf, size_t len, int flags)
{
    char msg_len[4];
    bzero(msg_len,4);
    sprintf(msg,"%d",(int)len);
    int row_req=(len+1+strlen(msg_len))/1000;
    while(1)
    {
     if(row_req==0)break;
     if(send_table_len<10)
     {
          //add to table
          --row_req;
          continue;
     }
     pthread_lock(&mutex_dummy1);
     pthread_cond_wait(&cond_send_full, &mutex_dummy1);
    }
}