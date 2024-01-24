#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

#define POTR 8888
#define LISTEN_MAX 128
#define BUFSIZE 128

/// 用单进程线程 实现并发

int main()
{
    /* 创建套接字句柄 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    bzero((void *)&localAddress, sizeof(localAddress));

    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(POTR);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t len = sizeof(localAddress);
    /* 将本地的端口 和IP 绑定 */
    int ret = bind(sockfd, (struct sockaddr *)&localAddress, len);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, LISTEN_MAX);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    fd_set readSet;
    /* 清空集合 */
    FD_ZERO(&readSet);
    /* 把监听的文件描述符添加到读集合中 让内核帮忙检测 */
    FD_SET(sockfd, &readSet);

#if 0
    /*超时  */
    struct timeval timeValue;
    timeValue.tv_sec = 5;
    ret = select(maxfd + 1, &readSet, NULL, NULL, &timeValue);

#else
    /* 备份读集合 */
    fd_set  tempreadSet = readSet; 
    bzero((void * )&tempreadSet, sizeof(tempreadSet));
    int maxfd = sockfd;
    while (1)
    {  
        tempreadSet = readSet; 
        ret = select(maxfd + 1, &tempreadSet, NULL, NULL, NULL);
        if (ret == -1)
        {
            perror("select error");
            break;
        }
#endif
        /* 如果sockfd 在 readSet里面 */
        if (FD_ISSET(sockfd, &tempreadSet))
        {
            int acceptfd = accept(sockfd, NULL, NULL);
            if (acceptfd == -1)
            {
                perror("accept error");
                break;
            }
            /* 将通信的句柄放到读集合 */
            FD_SET(acceptfd, &readSet);

            /* 更新maxfd的值 */
            maxfd = maxfd < acceptfd ? acceptfd : maxfd;
        }

   

        /* idx 是select里面的句柄 可能有通信 */
        for (int idx = 0; idx <= maxfd; idx++)
        {
            if (idx != sockfd && FD_ISSET(idx, &tempreadSet))
            {
                char buf[BUFSIZE];
                bzero((void *)buf, sizeof(buf));
                /* 里面一定有通信 一定是老客户 */
                /* */
                int readByte = read(idx, buf, sizeof(buf) - 1);
                if (readByte < 0)
                {
                    perror("read error");
                    /* 将该通信句柄从select的集合中删掉 */
                    FD_CLR(idx, &readSet);
                    close(idx);
                    /* 让下一个已经read的fd句柄 进行通信*/
                    continue;
                }
                else if (readByte == 0)
                {
                    printf("客户端断开....\n");
                    FD_CLR(idx, &readSet);
                    close(idx);
                    continue;
                }
                else
                {
                    printf("recv : %s\n", buf);
                    for(int jdx= 0; jdx < readByte; jdx++)
                    {
                        buf[jdx] = toupper(buf[jdx]);
                    }
                    write(idx, buf, sizeof(buf));
                    usleep(500);
                }
            }
        }
    }
    close(sockfd);

    return 0;
}