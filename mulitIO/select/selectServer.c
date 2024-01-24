#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <strings.h>

#define POTR 8888
#define LISTEN_MAX 128

/// 用单进程线程 实现并发

int main()
{
    /* 创建套接字句柄 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socker error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    bzero((void *)&localAddress, sizeof(localAddress));

    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(POTR);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t len = sizeof(localAddress);
    /* 将本地的端口 和IP 绑定 */
    int ret = bind(sockfd, (struct sockaddr_in *)&localAddress, len);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
   ret =  listen(sockfd, LISTEN_MAX);
   if(ret == -1)
    {
        perror("listen error");
        exit(-1);
    }














    return 0;
}