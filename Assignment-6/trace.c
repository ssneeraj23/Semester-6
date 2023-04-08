#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define PACKET_SIZE 4096
#define IP_HEADER_SIZE 20
#define ICMP_HEADER_SIZE 8
#define DEFAULT_TIMEOUT 5000 // in milliseconds
#define MAX_HOPS 30

struct icmp_packet
{
    struct icmphdr hdr;
    char msg[64 - sizeof(struct icmphdr)];
};

uint16_t checksum(uint16_t *buf, int nwords);
uint16_t checksum(uint16_t *buf, int nwords)
{
    uint32_t sum = 0;
    while (nwords > 1)
    {
        sum += *buf++;
        nwords -= 2;
    }
    if (nwords == 1)
    {
        sum += *(uint8_t *)buf;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s hostname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // resolve hostname to IP address
    struct hostent *host = gethostbyname(argv[1]);
    if (host == NULL)
    {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    printf("got host name\n");

    // create a raw socket
    int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("socket set\n");
    char packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));
    // set socket options
    const int on = 1;
    if (setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("options set\n");

    // prepare destination address
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    //dest_addr.sin_addr = *((struct in_addr *)host->h_addr);
     dest_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));

    // prepare ICMP echo request packet
    // struct icmp_packet icmp_pkt;
    // memset(&icmp_pkt, 0, sizeof(icmp_pkt));
    // icmp_pkt.hdr.type = ICMP_ECHO;
    // icmp_pkt.hdr.code = 0;
    // icmp_pkt.hdr.un.echo.id = getpid() & 0xFFFF;
    // icmp_pkt.hdr.un.echo.sequence = 1;
    // icmp_pkt.hdr.checksum = checksum((uint16_t *)&icmp_pkt, sizeof(icmp_pkt) / 2);

    // prepare source address
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = INADDR_ANY;
    src_addr.sin_port = 0;

    // bind socket to source address
    if (bind(sfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // set socket timeout
    struct timeval timeout;
    timeout.tv_sec = DEFAULT_TIMEOUT / 1000;
    timeout.tv_usec = (DEFAULT_TIMEOUT % 1000) * 1000;
    if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    

    struct icmphdr *icmp_hdr = (struct icmphdr *)packet;
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0; // set to 0 before computing checksum
    icmp_hdr->un.echo.id = getpid() & 0xFFFF;
    icmp_hdr->un.echo.sequence = 1;
    strncpy(packet + ICMP_HEADER_SIZE, "Hello, world!", PACKET_SIZE - ICMP_HEADER_SIZE);

    // compute checksum of ICMP header and data
    icmp_hdr->checksum = checksum((uint16_t *)icmp_hdr, PACKET_SIZE);

    // prepare IP header
    struct iphdr *ip_hdr = (struct iphdr *)packet;
    ip_hdr->version = 4;
    ip_hdr->ihl = 5;
    ip_hdr->tos = 0;
    ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + strlen("Hello, world!"));
    ip_hdr->id = htons(12345);
    ip_hdr->frag_off = 0;
    ip_hdr->ttl = 1; // set TTL to 1
    ip_hdr->protocol = IPPROTO_ICMP;
    ip_hdr->check = 0; // set to 0 before computing checksum
    ip_hdr->saddr = INADDR_ANY; // set to INADDR_ANY or the source IP address
    ip_hdr->daddr = dest_addr.sin_addr.s_addr;

    // compute checksum of IP header
    ip_hdr->check = checksum((uint16_t *)ip_hdr, sizeof(struct iphdr));

    // send packet
    if (sendto(sfd, packet, ntohs(ip_hdr->tot_len), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("packet sent baby\n");
    // loop until destination is reached or max hops exceeded
    // while (hops < MAX_HOPS)
    // {
    //     // send ICMP echo request packet
    //     if (sendto(sfd, &icmp_pkt, sizeof(icmp_pkt), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    //     {
    //         perror("sendto");
    //         exit(EXIT_FAILURE);
    //     }

    //     // receive ICMP echo reply packet
    //     char packet_buf[PACKET_SIZE];
    //     memset(packet_buf, 0, PACKET_SIZE);
    //     struct sockaddr_in src_addr;
    //     socklen_t src_addr_len = sizeof(src_addr);
    //     ssize_t recv_len = recvfrom(sfd, packet_buf, PACKET_SIZE, 0, (struct sockaddr *)&src_addr, &src_addr_len);
    //     if (recv_len < 0)
    //     {
    //         printf("%2d * * *\n", hops + 1);
    //     }
    //     else
    //     {
    //         printf("got here baby\n");
    //         // extract ICMP header and check if it's an echo reply packet
    //         struct icmphdr *icmp_hdr = (struct icmphdr *)(packet_buf + IP_HEADER_SIZE);
    //         if (icmp_hdr->type == ICMP_ECHOREPLY)
    //         {
    //             printf("%2d %s (%s) %0.3f ms\n", hops + 1, host->h_name, inet_ntoa(src_addr.sin_addr), (double)(recv_len - IP_HEADER_SIZE - ICMP_HEADER_SIZE) / (double)1000);
    //             if (src_addr.sin_addr.s_addr == dest_addr.sin_addr.s_addr)
    //             {
    //                 break;
    //             }
    //         }
    //     }

    //     // increment TTL and hop counter
    //     ttl++;
    //     hops++;
    //     if (setsockopt(sfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    //     {
    //         perror("setsockopt");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    // close socket
    close(sfd);
    return 0;
}
