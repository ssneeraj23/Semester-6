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
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#define max_len 50
#define bs 100
#define send_size 45
#define recs 45
#define MAX_LEN 1000
#define MAX_TOKENS 10
#define response_size 1000
#define BUF_SIZE 100
void get_my_time(char *buffer)
{
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *gmtime(&now);
    strftime(buffer, 80, "%a, %d %b %Y %X GMT", &tstruct);
    strcat(buffer, "\n");
}
struct http_request
{
    char url[MAX_LEN];
    int port;
    char method[MAX_LEN], path[MAX_LEN], host[MAX_LEN], date[MAX_LEN], if_mod[MAX_LEN], cont_type[MAX_LEN], con_lang[MAX_LEN];
    int connection;
    int content_len;
    char accept_lan[MAX_LEN], accept_req[MAX_LEN];
    char filename[MAX_LEN];
};
void remove_characters_after(char *str, char c)
{
    char *p = strchr(str, c);
    if (p)
    {
        *p = '\0';
    }
}
void parse_url(char *url, char *protocol, char *host, int *port, char *path)
{
    char temp_url[MAX_LEN];
    strcpy(temp_url, url);
    char *start = temp_url;
    char *end = strstr(temp_url, "://");
    if (end != NULL)
    {
        strncpy(protocol, start, end - start);
        protocol[end - start] = '\0';
        start = end + 3;
    }
    else
    {
        protocol[0] = '\0';
    }

    end = strchr(start, '/');
    if (end != NULL)
    {
        char *host_port = strndup(start, end - start);
        start = end;
        end = strchr(host_port, ':');
        if (end != NULL)
        {
            strncpy(host, host_port, end - host_port);
            host[end - host_port] = '\0';
            *port = atoi(end + 1);
        }
        else
        {
            strcpy(host, host_port);
            *port = 0;
        }
        free(host_port);
    }
    else
    {
        strcpy(host, start);
        *port = 0;
        start = "/";
    }
    remove_characters_after(start + 1, ':');
    strcpy(path, start + 1);
}
char **divide_string(char *input, char *delimiter)
{
    char *token;
    char **tokens = (char **)malloc(MAX_TOKENS * sizeof(char *));
    int i = 0;

    token = strtok(input, delimiter);
    while (token != NULL && i < MAX_TOKENS)
    {
        tokens[i] = (char *)malloc(MAX_LEN * sizeof(char));
        strcpy(tokens[i], token);
        i++;
        token = strtok(NULL, delimiter);
    }

    return tokens;
}
void get_my_time_2(char *buffer)
{
    time_t now = time(NULL);
    now -= 2 * 24 * 60 * 60;
    struct tm *tm = gmtime(&now);
    strftime(buffer, 100, "%a, %d %b %Y %T GMT", tm);
    strcat(buffer, "\n");
}
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
    return c;
}
char *get_file_extension(char *file)
{
    char *dot = strrchr(file, '.');
    if (!dot || dot == file)
        return "";
    return dot + 1;
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
int count_number_of_bytes(char buf[], char filename[])
{
    bzero(buf, BUF_SIZE);
    int t;
    int count = 0;
    FILE *infile = fopen(filename, "rb");
    if (infile == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }
    while ((t = fread(buf, 1, BUF_SIZE, infile)) > 0)
    {
        count += t;
    }
    fclose(infile);
    if (ferror(infile))
    {
        printf("Error reading file %s", filename);
    }
    return count;
}
int send_any_file(int newsockfd, char buf[], char filename[])
{
    bzero(buf, BUF_SIZE);
    FILE *infile, *outfile;
    char outfilename[] = "output";
    infile = fopen(filename, "rb");
    outfile = fopen(outfilename, "wb");
    if (infile == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }
    int t;
    while ((t = fread(buf, 1, BUF_SIZE, infile)) > 0)
    {
        send(newsockfd, buf, t, 0);
        fwrite(buf, 1, t, outfile);
    }
    fclose(infile);
    fclose(outfile);
    if (ferror(infile))
    {
        printf("Error reading file %s", filename);
    }
    bzero(buf, BUF_SIZE);
    return 1;
}
void send_put(struct http_request *req, int sockfd, char *buf)
{
    char request[response_size];
    char date_f[40];
    bzero(date_f, 40);
    bzero(request, response_size);
    strcpy(request, buf);
    strcat(request, "\n");
    strcat(request, "Host: ");
    strcat(request, req->host);
    strcat(request, "\n");
    strcat(request, "Connection: close\n");
    strcat(request, "Date: ");
    get_my_time(date_f);
    strcat(request, date_f);
    strcat(request, "Content-Language: en-US\n");
    strcat(request, "Content-type: ");
    char *extension = get_file_extension(req->filename);
    int file = open(req->filename, O_RDONLY);
    if (file == -1)
    {
        printf("not found bro||\n");
        return;
    }
    char *content_type = "text/*\n";
    if (strcmp(extension, "html") == 0)
    {
        content_type = "text/html\n";
    }
    else if (strcmp(extension, "pdf") == 0)
    {
        content_type = "application/pdf\n";
    }
    else if (strcmp(extension, "jpg") == 0)
    {
        content_type = "image/jpeg\n";
    }
    strcat(request, content_type);
    strcat(request, "Content-Length: ");
    char buff[BUF_SIZE] = {0};
    int x = count_number_of_bytes(buff, req->filename);
    char y[10];
    sprintf(y, "%d", x);
    strcat(request, y);
    strcat(request, "\n");
    send(sockfd, request, strlen(request), 0);
    send_any_file(sockfd, date_f, req->filename);
}

void send_get(struct http_request *req, int sockfd, char *buf)
{
    char request[response_size];
    char date_f[40];
    bzero(date_f, 40);
    bzero(request, response_size);
    strcpy(request, buf);
    strcat(request, "\n");
    strcat(request, "Host: ");
    strcat(request, req->host);
    strcat(request, "\n");
    strcat(request, "Connection: close\n");
    strcat(request, "Accept: ");
    char *extension = get_file_extension(req->path);
    int file = open(req->path, O_RDONLY);
    if (file == -1)
    {
        // file not found
        printf("not found bro\n");
        return;
    }
    char *content_type = "text/*\n";
    if (strcmp(extension, "html") == 0)
    {
        content_type = "text/html\n";
    }
    else if (strcmp(extension, "pdf") == 0)
    {
        content_type = "application/pdf\n";
    }
    else if (strcmp(extension, "jpg") == 0)
    {
        content_type = "image/jpeg\n";
    }
    strcat(request, content_type);
    strcat(request, "Accept-Language: en-US\n");
    strcat(request, "If-Modified-Since: ");
    get_my_time_2(date_f);
    strcat(request, date_f);
    send(sockfd, request, strlen(request), 0);
}

void par(char *input, struct http_request *req)
{
    // printf("%s\n", "hi");
    int len = strlen(input);
    char **tokens = divide_string(input, "\n");
    char requestt[MAX_LEN];
    strcpy(requestt, tokens[0]);
    sscanf(requestt, "%s", req->method);
    sscanf(requestt, "%*s %s", req->url);
    sscanf(requestt, "%*s %*s %s", req->filename);

    char protocol[MAX_LEN];
    parse_url(&req->url, protocol, &req->host, &req->port, &req->path);
    char *t;
    for (int i = 1; tokens[i]; i++)
    {
        char *end = strstr(tokens[i], " ");
        char *token = strtok(tokens[i], ":");
        if (token != NULL)
        {
            if (strcmp(token, "Host") == 0)
            {
                strcpy(req->host, end + 1);
            }
            else if (strcmp(token, "Connection") == 0)
            {
                if (strcmp(end + 1, "close") == 0)
                {
                    req->connection = 1;
                }
            }
            else if (strcmp(token, "Accept") == 0)
            {
                strcpy(req->accept_req, end + 1);
            }
            else if (strcmp(token, "Date") == 0)
            {
                strcpy(req->date, end + 1);
            }
            else if (strcmp(token, "Accept-Language") == 0)
            {
                strcpy(req->accept_lan, end + 1);
            }
            else if (strcmp(token, "If-Modifed-Since") == 0)
            {
                strcpy(req->if_mod, end + 1);
            }
            else if (strcmp(token, "Content-language") == 0)
            {
                strcpy(req->con_lang, end + 1);
            }
            else if (strcmp(token, "Content-length") == 0)
            {
                req->content_len = atoi(end + 1);
            }
            else if (strcmp(token, "Content-type") == 0)
            {
                strcpy(req->cont_type, end + 1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, clilen, l_port;
    l_port = atoi(argv[1]);
    struct sockaddr_in serv_addr;
    char servtime[64];
    char buff[2000];
    bzero(buff, 2000);
    for (int i = 0; i < 10; i++)
        servtime[i] = '\0';
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Cannot create socket\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(l_port);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr,
                 sizeof(serv_addr))) < 0)
    {
        perror("Unable to connect to server\n");
        exit(0);
    }
    printf("Connected\n");
    fgets(buff, 2000, stdin);
    struct http_request req;
    par(buff, &req);
    printf("%s\n", req.filename);
    printf("%s\n", req.method);
    if (strcmp("GET", req.method) == 0)
    {
        send_get(&req, sockfd, buff);
    }
    if (strcmp("PUT", req.method) == 0)
    {
        send_put(&req, sockfd, buff);
    }
    recv(sockfd, buff, 2000, 0);
    printf("%s\n", buff);
    bzero(buff, 2000);
    recv(sockfd, buff, 2000, 0);
    printf("%s\n", buff);
    printf("\n");
    close(sockfd);
    return 0;
}
