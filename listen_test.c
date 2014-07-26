//测试监听socket
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
typedef int bool;
//typedef 1 true;
//typedef 0 false;
#define true 1
#define false 0

static bool stop = false;
static void handle_term(int sig)
{
    stop = true;
}

int main(int argc, char* argv[])
{
    signal(SIGTERM, handle_term);

    if(argc < 3)
    {
        printf("usage's not invalid");
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock > 0);

    //创建一个IPv4 socket地址
    struct sockaddr_in cli_address;
    bzero(&cli_address, sizeof(cli_address));
    cli_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &cli_address.sin_addr);
    cli_address.sin_port = htons(port);

    int res = bind(sock, (struct sockaddr*) &cli_address, sizeof(cli_address));
    assert(res != -1);

    res = listen(sock, backlog);
    assert(res != -1);

    while(!stop)
    { 
        sleep(1);
    }

    close(sock);

    return 0;
}        


