#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <error.h>

#define SERVER_POTR 7777
#define MAX_LISTEN 128
#define SERVER_IP "172.16.104.91"
#define BUFSIZE 128

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
    }
    struct sockaddr_in serverAdress;
    memset(&serverAdress, 0, sizeof(serverAdress));

    /* 端口*/

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(SERVER_POTR);
    /* 地址转换 */
    int ret = inet_pton(AF_INET, SERVER_IP, (void *)&serverAdress.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton ERROR");
        exit(-1);
    }

    int rets = connect(sockfd, (struct sockaddr *)&serverAdress, sizeof(serverAdress));
    if (rets == -1)
    {
        perror("connect error");
        exit(-1);
    }
    char writebuf[BUFSIZE];
    memset(writebuf, 0, sizeof(writebuf));

    char recvebuf[BUFSIZE];
    memset(recvebuf, 0, sizeof(recvebuf));

    int readbytre = 0;
    while (1)
    {
#if 0
        strncpy(buf, " lilii", sizeof(buf) - 1);

        write(sockfd, buf, sizeof(buf));

        read(sockfd, buf, sizeof(buf));
     
       printf("recv: %s\n ",buf);
#endif
        /* 写 */
        printf("input:");
        scanf("%s", writebuf);
        write(sockfd, writebuf, strlen(writebuf) + 1);
        readbytre = read(sockfd, recvebuf, sizeof(recvebuf) - 1);
        if (readbytre < 0)
        {
            perror("read error");
        }
        else if(readbytre == 0)
        {
            printf("xxxxx\n");
        }
        else
        {
            printf("recv %s\n", recvebuf);
        }
    }

    close(sockfd);
    sleep(5);

    return 0;
}