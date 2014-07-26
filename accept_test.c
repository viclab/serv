#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("invalid usage");
        return 1;
    }
    
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sin.sin_addr);
    sin.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
    assert(ret != -1);
    
    ret = listen(sock, 5);
    assert(ret != -1);

    sleep(20);
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
    if (connfd < 0)
        printf("accept erro");
    else
    {
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip:%s and port:%d", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port));
        close(connfd);
    }

    close(sock);

    return 0;
}
