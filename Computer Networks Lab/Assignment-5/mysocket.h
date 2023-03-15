#ifndef	MYSOCKET
#define	MYSOCKET
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

int my_socket(int domain, int type, int protocol);
int my_bind(int mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_listen(int mysockfd, int backlog);
int my_accept(int mysockfd, struct sockaddr *addr, socklen_t *addrlen);
int my_connect(int mysockfd, const struct sockaddr *addr, socklen_t addrlen);
int my_send(int mysockfd, const void *buf, size_t len, int flags);
int my_recv(int mysockfd, void *buf, size_t len, int flags);
int my_close(int mysockfd);
int min(int a, int b);



#endif