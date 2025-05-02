#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; //any address를 위한 Enough space
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if(argc!=2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd=Open_listenfd(argv[1]);
    while(1){
        clientlen=sizeof(struct sockaddr_storage);
        connfd=Accept(listenfd, (SA *)&clientaddr, &clientlen);//connfd = connected file descriptor 연결된 파일 디스크립터
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("connected to (%s, %s)\n", client_hostname, client_port);
        echo(connfd);
        Close(connfd);
    }
    exit(0);
}

// echoserveri.c의 i는 iterative의 약자
// i = iterative = 반복형 서버
// 즉, 이 파일은 반복(iterative) 구조로 동작하는 echo 서버임을 의미