#include "csapp.h"

int main(int argc, char **argv){
    int clientfd;//클라이언트 파일 디스크립트
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if(argc!=3){//프로그램 실행 시 전달된 인자의 개수가 정확히 3개인지 확인하는 조건문
        //사용자가 실행할 때 인자를 제대로 안 넣었을 경우, 오류 메시지 출력하고 프로그램을 종료시키기 위해 필요
        fprintf(stderr, "usage: %s <host> <post>\n", argv[0]);
        exit(0);
    }
    host=argv[1];
    port=argv[2];

    clientfd=Open_clientfd(host,port);
    Rio_readinitb(&rio, clientfd);//Robust I/O_read initialize buffered 읽기 버퍼 초기화

    while(Fgets(buf, MAXLINE, stdin)!=NULL){
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    Close(clientfd);
    exit(0);
}