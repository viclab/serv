#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BUFSIZE 100

int main(void)
{
    int res, pid, nwr;
    int pipefd[2];
    char buf[BUFSIZE];
    res = pipe(pipefd);
    assert(res >= 0);
    
    if ((pid = fork()) < 0) {
        printf("fork error");
        return -1;
    }else if (pid = 0) {
        close(pipefd[1]);
        if ((nwr = read(pipefd[0], buf, BUFSIZE)) <= 0)
            printf("read error");
        printf("%s", buf);
        close(pipefd[0]);
    }
    
    close(pipefd[0]);
    sprintf(buf, "Welcome to Vic Cloud Server\n");
    
    if ((nwr = write(pipefd[1], buf, strlen(buf))) <= 0)
        printf("write error");
    close(pipefd[1]);
    
    return 0;
    
}
