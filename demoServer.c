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

#define SERVER_POTR 7776
#define MAX_LISTEN 128
#define LOCAL_IPADDRESS "172.16.104.91"
#define BUFSIZE 128

void signhander(int sing)
{

}


int main()
{
    /* 注册信号 回收资源 */
    // signal(SIGINT, signhander);
    // signal(SIGQUIT,signhander );
    // signal(SIGPIPE, signhander);

    /* 创建套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }
#if 0
    /* 绑定 */
    struct   sockaddr localsockaddr;
#else
    /* 本地地址 */
    struct sockaddr_in localsockaddr;

#endif
    memset(&localsockaddr, 0, sizeof(localsockaddr));
    /* 地址族 */
    localsockaddr.sin_family = AF_INET;
    /* 端口 转成大端 */
    localsockaddr.sin_port = htons(SERVER_POTR);
    /* ip地址 转成大端     INADDR_ANY 为默认地址0x00000000 */
    localsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 不是默认地址 */
    // inet_pton(AF_INET, LOCAL_IPADDRESS, &(localsockaddr.sin_addr.s_addr));

    /* 绑定 */
    socklen_t localSize = sizeof(localsockaddr);
    int ret = bind(sockfd, (struct sockaddr *)&localsockaddr, localSize);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));

    socklen_t clientLen = 0;
    /* 连接 */
    int aret = accept(sockfd, (struct sockaddr *)&clientAddress, &clientLen);
    if (aret == -1)
    {
        perror("accept error");
    }

    char buf[BUFSIZE];
    memset(buf, 0, sizeof(buf));

    char replebuf[BUFSIZE];
    memset(replebuf, 0, sizeof(replebuf));

    int readByte = 0;
    while (1)
    {
        readByte = read(aret, buf, sizeof(buf));
        if(readByte <= 0)
        {
            perror("read error");
            close(aret);
            break;
            exit(-1);
        }
        else
        {
            printf("buf : %s\n",buf);

            sleep(3);
            strncpy(replebuf," 你好 lili", sizeof(replebuf) - 1);
             write(aret, replebuf, sizeof(replebuf));

        }
    }

    close(sockfd);

    return 0;
}
