#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1000

int main(int argc, char * argv[])
{
    if (argc <= 2)
    {
        printf("invalid usage");
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in sin, cli;
    sin.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sin.sin_addr);
    sin.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    int ret = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    assert(ret != -1);
    ret = listen(sock, 5);
    assert(ret != -1);

    socklen_t cli_len = sizeof(cli);
    int connfd = accept(sock, (struct sockaddr*)&cli, &cli_len);
    if (connfd < 0)
        printf("accpet error");
    else
    {
        int i;
        size_t nwrite;
        char buf[BUFSIZE];
        //strcpy(buf, "Welcome to Vic Server\n");
        for (i = 0; i < 10; ++i)
        {
            sprintf(buf, "[%d]:Welcome to Cloud VIC Server#\n", i);
            //if ((nwrite = write(connfd, buf, strlen(buf))) <= 0)
            if ((nwrite = send(connfd, buf, strlen(buf), 0)) <= 0)
                printf("write error");
                

        }
        close(connfd);
        
    }
    close(sock);

    return 0;
}
