#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define bs 200
#define file_name "users.txt"
#define c_invalid "$$$$"
#define com_error "####"
#define send_size 45

int is_valid(char *buf, int size)
{
    FILE *f;
    f = fopen(file_name, "r");
    char t[27];
    while (fgets(t, 27, f) != NULL)
    {

        for (int i = 0; i < 27; ++i)
        {
            if (t[i] == '\n')
            {
                t[i] = '\0';
                break;
            }
            if (t[i] == '\0')
                break;
        }
        if (strcmp(buf, t) == 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

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

int main()
{
    int sockfd, newsockfd; /* Socket descriptors */
    int clilen;
    struct sockaddr_in cli_addr, serv_addr;
    char buf[bs];
    char recv_buf[45];
    for (int i = 0; i < bs; i++)
        buf[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //printf("Cannot create socket\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(20000);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        //printf("Unable to bind local address\n");
        exit(0);
    }
    listen(sockfd, 5);
    while (1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           &clilen);

        if (newsockfd < 0)
        {
            //printf("Accept error\n");
            exit(0);
        }
        if (fork() == 0)
        {
            int clo = 0;
            close(sockfd);
            strcpy(buf, "LOGIN");
            send(newsockfd, buf, strlen(buf) + 1, 0);
            for (int i = 0; i < bs; i++)
                buf[i] = '\0';
            if (is_valid(buf, recv(newsockfd, buf, 26, 0)) == 0)
            {
                strcpy(buf, "NOT-FOUND");
                buf[9] = '\0';
                send(newsockfd, buf, 10, 0);
                if (recv(newsockfd, buf, 10, 0) <= 0)
                    exit(0);
            }
            strcpy(buf, "FOUND");
            buf[5] = '\0';
            send(newsockfd, buf, 6, 0);
            while (1)
            {

                for (int i = 0; i < 45; ++i)
                    recv_buf[i] = '\0';
                int rs = recv(newsockfd, recv_buf, 45, 0);
                if (rs <= 0)
                {
                    clo = 1;
                    break;
                }
                for (int i = 0; i < bs; ++i)
                    buf[i] = '\0';
                strcat(buf, recv_buf);
                while (recv_buf[rs - 1] != '\0')
                {
                    rs = recv(newsockfd, recv_buf, 45, 0);
                    strcat(buf, recv_buf);
                }
                int arg_start;
                int first_char = 0;
                // --note see if i+3 is in bound like just pw should give error
                for (int i = 0; i < bs; ++i)
                {
                    if (first_char == 0 && buf[i] == ' ')
                        continue;
                    if (first_char == 0 && buf[i] != ' ')
                    {
                        first_char = 1;
                        if (i + 3 < bs)
                        {
                            if ((buf[i] == 'p' && buf[i + 1] == 'w') && (buf[i + 2] == 'd' && (buf[i + 3] == ' ' || buf[i + 3] == '\0')))
                            {
                                // call pwd
                                for (int i = 0; i < bs; ++i)
                                    buf[i] = '\0';
                                getcwd(buf, bs);
                                send_cli(newsockfd, buf, strlen(buf) + 1);
                                //printf("pwd is  %s\n", buf);
                                break;
                                // send result
                            }
                            if ((buf[i] == 'd' && buf[i + 1] == 'i') && (buf[i + 2] == 'r' && (buf[i + 3] == ' ' || buf[i + 3] == '\0')))
                            {
                                arg_start = i + 4;
                                struct dirent *de;
                                char res[bs];
                                bzero(res, bs);
                                while (arg_start < bs && (buf[arg_start] == ' ' || buf[arg_start] == '\0'))
                                {
                                    ++arg_start;
                                }
                                if (arg_start == bs)
                                {
                                    // call dir with no args
                                    DIR *dr = opendir(".");
                                    if (dr == NULL) // opendir returns NULL if couldn't open directory
                                    {
                                        //printf("Could not open current directory");
                                        bzero(buf, bs);
                                        strcpy(buf, com_error);
                                        send_cli(newsockfd, buf, strlen(buf) + 1);
                                    }
                                    else
                                    {
                                        while ((de = readdir(dr)) != NULL)
                                        {
                                            //
                                            char f[2];
                                            f[0] = '\n';
                                            f[1] = '\0';
                                            strcat(de->d_name, f);
                                            strcat(res, de->d_name);
                                        }
                                        send_cli(newsockfd, res, strlen(res) + 1);
                                    }
                                    //printf("%s is dir\n", res);
                                    closedir(dr);
                                }
                                else
                                {
                                    DIR *dr = opendir(buf + arg_start);
                                    if (dr == NULL) // opendir returns NULL if couldn't open directory
                                    {
                                        //printf("Could not open current directory");
                                        bzero(buf, bs);
                                        strcpy(buf, com_error);
                                        send_cli(newsockfd, buf, strlen(buf) + 1);
                                        // send c_error
                                    }
                                    else
                                    {
                                        while ((de = readdir(dr)) != NULL)
                                        {
                                            char f[2];
                                            f[0] = '\n';
                                            f[1] = '\0';
                                            strcat(de->d_name, f);
                                            strcat(res, de->d_name);
                                        };
                                        send_cli(newsockfd, res, strlen(res) + 1);
                                        //printf("%s is dir\n", res);
                                    }
                                    closedir(dr);
                                }
                                break;
                                // call dir with args
                            }
                            // send c_invalid
                            // bzero(buf, bs);
                            // strcpy(buf, c_invalid);
                            // send_cli(newsockfd, buf, strlen(buf) + 1);
                            // break;
                        }
                        if (i + 2 < bs)
                        {
                            if ((buf[i] == 'c' && buf[i + 1] == 'd') && (buf[i + 2] == ' ' || buf[i + 2] == '\0'))
                            {
                                arg_start = i + 3;
                                while (arg_start < bs && (buf[arg_start] == ' ' || buf[arg_start] == '\0'))
                                {
                                    ++arg_start;
                                }
                                if (arg_start == bs)
                                {
                                    // call cd with no args
                                    chdir("/home");
                                    char suc[1] = "\0";
                                    send_cli(newsockfd, suc, 1);
                                    break;
                                }

                                // call cd with arg, it just see till \0
                                //printf("arg is %s\n", buf + arg_start);
                                if (chdir(buf + arg_start) == 0)
                                {
                                    for (int i = 0; i < bs; ++i)
                                        buf[i] = '\0';
                                    getcwd(buf, bs);
                                    //printf("cd after pwd is %s\n", buf);
                                    char suc[1] = "\0";
                                    send_cli(newsockfd, suc, 1);
                                }
                                else
                                {
                                    // send comm_error
                                    bzero(buf, bs);
                                    strcpy(buf, com_error);
                                    send_cli(newsockfd, buf, strlen(buf) + 1);
                                }
                            }
                        }
                        // send c_invalid
                        bzero(buf, bs);
                        strcpy(buf, c_invalid);
                        send_cli(newsockfd, buf, strlen(buf) + 1);
                        break;
                    }
                    else
                    {
                        // send c_invalid
                        bzero(buf, bs);
                        strcpy(buf, c_invalid);
                        send_cli(newsockfd, buf, strlen(buf) + 1);
                        break;
                    }
                }
                // //printf("%s \nisreceived\n",buf);
            }

            close(newsockfd);
            exit(0);
        }
        close(newsockfd);
    }
    return 0;
}
