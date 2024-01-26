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
#include <sys/epoll.h>
#include <ctype.h>

#define SERVER_POTR 7777
#define MAX_LISTEN 128
#define LOCAL_IPADDRESS "172.16.104.91"
#define BUFSIZE 128

int main()
{

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

    /* 设置端口复用 */
    int enableOpt = 1;
    int set = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableOpt, sizeof(enableOpt));
    if (set == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

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

    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll create error");
        exit(-1);
    }
    /* 将sockfd 添加到红黑树的实例里面 */
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    /* 将sockfd 添加到实例里面 */
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if (ret == -1)
    {
        perror("ctl error");
        exit(-1);
    }
    int nums = 0;
    int readByte = 0;
    while (1)

    {
        struct epoll_event events[BUFSIZE];
        memset(&events, 0, sizeof(events));
        nums = epoll_wait(epfd, events, BUFSIZE, -1);
        if (nums == -1)
        {
            perror("wait error ");
            exit(-1);
        }

        /* 程序到这里 有2种情况 超时  有监听数据来*/
        for (int idx = 0; idx < nums; idx++)
        {
            int fd = events[idx].data.fd;
            if (fd == sockfd)
            {
                /* 有连接 */
                int connfd = accept(sockfd, NULL, NULL);
                if (connfd == -1)
                {
                    perror("connfd error");
                    exit(-1);
                }
                struct epoll_event conn_event;
                memset(&conn_event, 0, sizeof(conn_event));
                conn_event.data.fd = connfd;
                conn_event.events = EPOLLIN;

                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &conn_event);
                if (ret == -1)
                {
                    perror("epoll_ctl error");
                    continue;
                }
            }
            else
            {
                /* 有数据通信 */
                char buf[BUFSIZE];
                memset(buf, 0, sizeof(buf));
                
                readByte = read(fd, buf, sizeof(buf) - 1);
                if (readByte == 0)
                {
                    printf("客户端下线....\n");
                    /* 将该文件句柄从红黑树删除 */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                else if (readByte < 0)
                {
                    printf("read error\n");
                    /* 将该文件句柄从红黑树删除 */
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
                else
                {
                    printf("recv %s\n", buf);
                    for (int jdx = 0; jdx < readByte; jdx++)
                    {
                        buf[jdx] = toupper(buf[jdx]);
                    }
                    write(fd, buf, readByte);

                    usleep(300);
                }
            }
        }
    }

    close(sockfd);

    return 0;
}
