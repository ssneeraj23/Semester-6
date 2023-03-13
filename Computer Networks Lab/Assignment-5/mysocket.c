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

#define SOCK_MyTCP 18
int min(int a, int b)
{
     return a < b ? a : b;
}
typedef struct
{
     int sock;
} MyTCP;

MyTCP curr_socket;
MyTCP main_socket;
char **send_message;
char **received_message;
int send_table_len, recv_table_len;
int send_table_index, recv_table_index;
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

int recv_cli(char *buf, int newsockfd)
{
     int recs = 100;
     char msg_len_str[5];
     bzero(msg_len_str, 5);
     char t[1];
     char recv_buf[recs + 1];
     for (int i = 0; i < recs; ++i)
          recv_buf[i] = '\0';
     int c = 0;
     int rs;
     while (1)
     {
          rs=recv(newsockfd, t, 1, 0);
          if(rs<=0)return rs;
          strcat(msg_len_str, t);
          if (t[0] == '\r')
               break;
     }
     strcat(buf, msg_len_str);
     int msg_len = atoi(msg_len_str);
     int tbread = msg_len;
     while (1)
     {
          rs = recv(newsockfd, recv_buf, min(recs, tbread), 0);
          if(rs<=0)return rs;
          recv_buf[rs] = '\0';
          strcat(buf, recv_buf);
          tbread -= rs;
          c += rs;
          if (tbread == 0)
               break;
     }
     c += strlen(msg_len_str);
     return c;
}

void *R(void *arg)
{
     int rs;
     while (1)
     {
          pthread_mutex_lock(&mutex_recv);
          pthread_cond_wait(&cond_recv, &mutex_recv);
          while (1)
          {
               if (recv_table_len != 10)
               {
                     rs=recv_cli( received_message[(recv_table_index+recv_table_len)%10], curr_socket.sock);
                     if(rs<=0)break;
                     ++recv_table_len;
               }
          }
     }
     return NULL;
}

void *S(void *arg)
{
     int tbsend = 0;
     int msg_len, i;
     char msg_len_str[4];
     while (1)
     {
          pthread_mutex_lock(&mutex_send);
          pthread_cond_wait(&cond_send, &mutex_send);
          while (1)
          {
               if (send_table_len == 0)
               {
                    sleep(1);
               }

               if (tbsend == 0)
               {
                    bzero(msg_len_str, 4);
                    i = 0;
                    while (send_message[send_table_index][i] != '\r')
                    {
                         msg_len_str[i] = send_message[send_table_index][i];
                         ++i;
                    }
                    msg_len = atoi(msg_len_str);
                    tbsend = msg_len;
                    if (1000 - i - 1 >= tbsend)
                    {
                         send(curr_socket.sock, send_message[send_table_index] + i + 1, tbsend, 0);
                         tbsend = 0;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                    }
                    else
                    {
                         send(curr_socket.sock, send_message[send_table_index] + i + 1, 1000 - i - 1, 0);
                         tbsend = tbsend - 1000 + i + 1;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                    }
               }
               else
               {
                    if (tbsend <= 1000)
                    {
                         send(curr_socket.sock, send_message[send_table_index], tbsend, 0);
                         tbsend = 0;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                    }
                    else
                    {
                         send(curr_socket.sock, send_message[send_table_index], 1000, 0);
                         tbsend -= 1000;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                    }
               }
          }
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
               bzero(send_message[i], 1000);
          }
          received_message = (char **)malloc(10 * sizeof(char *));
          for (int i = 0; i < 10; i++)
          {
               received_message[i] = (char *)malloc(5005 * sizeof(char));
               bzero(received_message[i], 5005);
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
     int newsockfd;
     newsockfd = accept(mysockfd.sock, (struct sockaddr *)&cli_addr, clilen);
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
     else
     {
          // tharavatha rastha
     }
}

int my_send(MyTCP mysockfd, const void *buf, size_t len, int flags)
{
     char msg_len[4];
     bzero(msg_len, 4);
     sprintf(msg_len, "%d", (int)len);
     int row_req = (len + 1 + strlen(msg_len)) / 1000;
     int first = 0;
     int ind = 0;
     while (1)
     {
          if (row_req == 0)
               break;
          if (send_table_len < 10)
          {
               // add to table
               if (first == 0)
               {
                    first = 1;
                    strcpy(send_message[(send_table_index + send_table_len) % 10], msg_len);
                    strcat(send_message[send_table_index], "\r");

                    if (len > 1000 - strlen(msg_len) - 1)
                    {
                         strncat(send_message[(send_table_index + send_table_len) % 10], buf, 1000 - strlen(msg_len) - 1);
                         ind = 1000 - strlen(msg_len) - 1;
                    }
                    else
                    {
                         strcat(send_message[(send_table_index + send_table_len) % 10], buf);
                    }
                    send_table_len++;
               }
               else
               {
                    if (len - ind > 1000)
                    {
                         strncpy(send_message[(send_table_index + send_table_len) % 10], buf + ind, 1000);
                         ind = ind + 1000;
                    }
                    else
                    {
                         strcpy(send_message[(send_table_index + send_table_len) % 10], buf + ind);
                    }
                    send_table_index++;
               }
               --row_req;
               continue;
          }
          return len;
          pthread_mutex_lock(&mutex_dummy1);
          pthread_cond_wait(&cond_send_full, &mutex_dummy1);
     }
}

int my_recv(MyTCP mysockfd, void *buf, size_t len, int flags)
{

     char msg_len[4];
     bzero(msg_len, 4);
     int i = 0;
     while (received_message[recv_table_index][i] != '\r')
     {
          msg_len[i] = received_message[recv_table_index % 10][i];
          i++;
     }
     int num_of_bytes = atoi(msg_len);

     while (1)
     {
          if (recv_table_len <= 0)
          {
               sleep(1);
          }
          else
          {
               if (len >= num_of_bytes)
               {
                    strcpy(buf, received_message[recv_table_index % 10] + i + 1);
               }
               else
               {
                    strncpy(buf, received_message[recv_table_index % 10] + i + 1, len);
               }
               bzero(received_message[recv_table_index % 10], 5005);
               --recv_table_len;
               recv_table_index++;
          }
     }
}