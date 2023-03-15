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

int curr_socket;
int main_socket;
char **send_message;
char **received_message;
int send_table_len = 0, recv_table_len = 0;
int send_table_index = 0, recv_table_index = 0;
pthread_t Recvt, Sendt;

pthread_cond_t cond_recv = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_send = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_send_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t send_cond_wait = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_recv = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_send = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_dummy1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recvt_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sendt_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_wait = PTHREAD_MUTEX_INITIALIZER;

int my_socket(int domain, int type, int protocol);
int my_bind(int mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_listen(int mysockfd, int backlog);
int my_accept(int mysockfd, struct sockaddr *addr, socklen_t *addrlen);
int my_connect(int mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_send(int mysockfd, const void *buf, size_t len, int flags);
int my_recv(int mysockfd, void *buf, size_t len, int flags);
int my_close(int mysockfd);
int min(int a, int b);

int recv_cli(char *buf, int newsockfd)
{
     int recs = 100;
     char msg_len_str[5];
     bzero(msg_len_str, 5);
     char t[2];
     t[1] = '\0';
     char recv_buf[recs + 1];
     for (int i = 0; i < recs; ++i)
          recv_buf[i] = '\0';
     int c = 0;
     int rs = 0;
     while (1)
     {
          rs = recv(newsockfd, t, 1, 0);
          if (rs <= 0)
               return rs;
          if (t[0] == '?')
          {
               break;
          }
          strcat(msg_len_str, t);
     }
     int msg_len = atoi(msg_len_str);
     strcat(msg_len_str, t);
     strcat(buf, msg_len_str);
     int tbread = msg_len;
     while (1)
     {
          rs = recv(newsockfd, recv_buf, min(recs, tbread), 0);
          if (rs <= 0)
               return rs;
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
          char temp[5005];
          while (1)
          {
               pthread_mutex_lock(&recvt_lock);
               bzero(temp, 5005);
               if (recv_table_len != 10)
               {
                    pthread_mutex_unlock(&recvt_lock);
                    rs = recv_cli(temp, curr_socket);
                    if (rs <= 0)
                    {
                         break;
                    }
                    pthread_mutex_lock(&recvt_lock);
                    strcpy(received_message[(recv_table_index + recv_table_len) % 10], temp);
                    ++recv_table_len;
               }
               pthread_mutex_unlock(&recvt_lock);
          }
          pthread_mutex_unlock(&mutex_recv);
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
          while (1)
          {
               pthread_mutex_lock(&sendt_lock);
               if (send_table_len == 0)
               {
                    pthread_mutex_unlock(&sendt_lock);
                    sleep(1);
                    continue;
               }

               if (tbsend == 0)
               {
                    sleep(1);
                    bzero(msg_len_str, 4);
                    i = 0;
                    while (send_message[send_table_index][i] != '?')
                    {
                         msg_len_str[i] = send_message[send_table_index][i];
                         ++i;
                    }
                    msg_len = atoi(msg_len_str);
                    tbsend = msg_len;
                    send(curr_socket, send_message[send_table_index], i + 1, 0);
                    if (1000 - i - 1 >= tbsend)
                    {
                         int ss = send(curr_socket, send_message[send_table_index] + i + 1, tbsend, 0);
                         tbsend = 0;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                         pthread_mutex_lock(&send_wait);
                         pthread_cond_broadcast(&send_cond_wait);
                         pthread_mutex_unlock(&send_wait);
                    }
                    else
                    {
                         int ss = send(curr_socket, send_message[send_table_index] + i + 1, 1000 - i - 1, 0);
                         tbsend = tbsend - 1000 + i + 1;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                         pthread_mutex_lock(&send_wait);
                         pthread_cond_broadcast(&send_cond_wait);
                         pthread_mutex_unlock(&send_wait);
                    }
               }
               else
               {
                    if (tbsend <= 1000)
                    {
                         send(curr_socket, send_message[send_table_index], tbsend, 0);
                         tbsend = 0;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                         if (tbsend == 0)
                         {
                              pthread_mutex_lock(&send_wait);
                              pthread_cond_broadcast(&send_cond_wait);
                              pthread_mutex_unlock(&send_wait);
                         }
                    }
                    else
                    {
                         send(curr_socket, send_message[send_table_index], 1000, 0);
                         tbsend -= 1000;
                         send_table_index = (send_table_index + 1) % 10;
                         --send_table_len;
                         if (tbsend == 0)
                         {
                              pthread_mutex_lock(&send_wait);
                              pthread_cond_broadcast(&send_cond_wait);
                              pthread_mutex_unlock(&send_wait);
                         }
                    }
               }
               pthread_mutex_unlock(&sendt_lock);
          }
     }
     return NULL;
}

int min(int a, int b)
{
     return a < b ? a : b;
}
int my_socket(int domain, int type, int protocol)
{

     if (type != SOCK_MyTCP)
     {
          return -1;
     }
     else
     {

          int sock;
          sock = socket(domain, SOCK_STREAM, protocol);
          if (sock < 0)
          {
               sock = -1;
               return sock;
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
          curr_socket = sock;
          main_socket = sock;
          pthread_create(&Sendt, &attr, R, NULL);
          pthread_create(&Recvt, &attr, S, NULL);
          send_table_len = 0;
          recv_table_len = 0;
          send_table_index = 0;
          recv_table_index = 0;
          return sock;
     }
}

int my_bind(int mysockfd, const struct sockaddr *addr, socklen_t addrlen)
{
     return bind(mysockfd, addr, addrlen);
}

int my_listen(int mysockfd, int backlog)
{
     int ret = listen(mysockfd, backlog);
     if (ret < 0)
     {
          perror("Error in listen");
          return -1;
     }
     return ret;
}

int my_accept(int mysockfd, struct sockaddr *cli_addr, socklen_t *clilen)
{
     int newsockfd;
     newsockfd = accept(mysockfd, (struct sockaddr *)&cli_addr, clilen);
     if (newsockfd < 0)
     {
          newsockfd = -1;
          return newsockfd;
     }

     curr_socket = newsockfd;

     sleep(1);
     pthread_mutex_lock(&mutex_recv);
     pthread_cond_broadcast(&cond_recv);
     pthread_mutex_unlock(&mutex_recv);
     return newsockfd;
}

int my_connect(int mysockfd, const struct sockaddr *addr, socklen_t addrlen)
{

     int res = connect(mysockfd, addr, addrlen);
     if (res == -1)
     {
          perror("Connection failed");
          return -1;
     }
     curr_socket = mysockfd;
     pthread_mutex_lock(&mutex_recv);
     pthread_cond_broadcast(&cond_recv);
     pthread_mutex_unlock(&mutex_recv);
     return 0;
}

int my_close(int mysockfd)
{
     if (mysockfd == main_socket)
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

          close(mysockfd);
     }
     else
     {
          close(mysockfd);
          pthread_mutex_lock(&sendt_lock);
          pthread_mutex_lock(&recvt_lock);
          send_table_len = 0;
          recv_table_len = 0;
          send_table_index = 0;
          recv_table_index = 0;
          pthread_mutex_unlock(&sendt_lock);
          pthread_mutex_unlock(&recvt_lock);
     }
}

int my_send(int mysockfd, const void *buf, size_t len, int flags)
{
     char msg_len[4];
     bzero(msg_len, 4);
     sprintf(msg_len, "%d", (int)len);
     int biglen = len + 1 + strlen(msg_len);
     int row_req;
     if (biglen % 1000 == 0)
          row_req = biglen / 1000;
     else
          row_req = (biglen / 1000) + 1;
     int first = 0;
     int ind = 0;
     while (1)
     {
          if (row_req == 0)
               break;
          pthread_mutex_lock(&sendt_lock);
          if (send_table_len < 10)
          {
               // add to table
               if (first == 0)
               {
                    first = 1;
                    strcpy(send_message[(send_table_index + send_table_len) % 10], msg_len);
                    strcat(send_message[send_table_index], "?");

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
                    send_table_len++;
               }
               --row_req;
          }
          pthread_mutex_unlock(&sendt_lock);
     }
     pthread_mutex_lock(&send_wait);
     pthread_cond_wait(&send_cond_wait, &send_wait);
     return len;
}

int my_recv(int mysockfd, void *buff, size_t len, int flags)
{
     char *buf = (char *)buff;
     sleep(3);
     char msg_len[4];
     int i = 0;
     int num_of_bytes;
     while (1)
     {
          pthread_mutex_lock(&recvt_lock);
          if (recv_table_len <= 0)
          {
               pthread_mutex_unlock(&recvt_lock);
               sleep(1);
          }
          else
          {
               bzero(msg_len, 4);
               i = 0;
               while (received_message[recv_table_index][i] != '?')
               {
                    msg_len[i] = received_message[recv_table_index % 10][i];
                    i++;
               }
               num_of_bytes = atoi(msg_len);
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
               recv_table_index = (recv_table_index + 1) % 10;
               pthread_mutex_unlock(&recvt_lock);
               break;
          }
     }
     return len;
}