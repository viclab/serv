#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
//#include <unistd.h>
#include <errno.h>
#include <malloc.h>
 
#define MAXEVENTS 200
#define BUFSIZE 50
 
int set_nonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
 
int main(int argc, char * argv[])
{
    if (argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", 
                argv[0]);
        return -1;
    }
 
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    
    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sin.sin_addr);
    sin.sin_port = htons(port);
 
    //创建并绑定socket
    int sock, ret;
    int epfd;
    struct epoll_event event;
    struct epoll_event *events;
 
    sock = socket(PF_INET, SOCK_STREAM, 0);
    assert (sock >= 0);
 
    ret = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    assert(ret != -1);
 
    ret = listen(sock, 5);
    assert(ret != -1);
 
    //set non-blocking
    ret = set_nonblocking(sock);
    assert(ret != -1);
 
    epfd = epoll_create(MAXEVENTS);
    assert (epfd != -1);
 
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLET;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
    assert(ret != -1);
 
    events = (struct epoll_event*)malloc(sizeof(struct epoll_event)
            * MAXEVENTS);
    assert(events != NULL);
    
    while (1)
    {
        int n, i;
        n = epoll_wait(epfd, events, MAXEVENTS, -1);
        for (i = 0; i < n; ++i)
        {
            if ((events[i].events & EPOLLERR)
                    || (events[i].events & EPOLLHUP)
                    || !(events[i].events & EPOLLIN))
            {
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            }
            else if (sock == events[i].data.fd)
            {
                //struct sockadd_in cli;
                //socklen_t cli_len;
                int connfd, res;
                //cli_len = sizeof(struct sockadd_in);
                struct sockaddr_in client;
                socklen_t cli_len = sizeof(client);
                connfd = accept(sock, (struct sockaddr*)&client, &cli_len);
                if (connfd == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    else
                    {
                        perror("accept error");
                        break;
                    }
                }
                
                char host[BUFSIZE], serv[BUFSIZE];
                res = getnameinfo((struct sockaddr*)&client, cli_len,
                        host, sizeof(host),
                        serv, sizeof(serv),
                        NI_NUMERICHOST | NI_NUMERICSERV);
                
                if (res ==0)
                    printf("Accepted connection on descriptor %d "
                        "(host=%s, port=%s)\n", connfd, host, serv);
                
                res = set_nonblocking(connfd);
                assert(res != -1);
 
                event.data.fd = connfd;
                event.events = EPOLLIN | EPOLLET;
                res = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                assert(res != -1);
 
            }
            else {
                ssize_t count;
                char buf[BUFSIZE + 1];
                //buf[BUFSIZE] = '\0';
                memset(buf, '\0', BUFSIZE + 1);
                while ((count = read(events[i].data.fd, buf, BUFSIZE)) > 0)
                {
                    printf("Data from client:%s", buf);
                    memset(buf, '\0', BUFSIZE + 1);
                }
            }
        }
    }
 
 
    return 0;
}
