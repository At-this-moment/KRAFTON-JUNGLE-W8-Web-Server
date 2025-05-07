#include "csapp.h" // tiny 웹서버에서 사용하는 보조 함수들과 상수를 포함한 헤더파일

// 함수 선언: 코드 아래쪽에서 정의된 함수들을 미리 알림
void doit(int fd); // 클라이언트 요청을 처리하는 핵심 함수
void read_requesthdrs(rio_t *rp); // HTTP 요청 헤더를 읽는 함수
int parse_uri(char *uri, char *filename, char *cgiargs); // 요청된 URI를 정적/동적 요청으로 파싱하는 함수
void serve_static(int fd, char *filename, int filesize, int is_head); // 정적 파일을 클라이언트에게 전송
void get_filetype(char *filename, char *filetype); // 파일 이름으로부터 MIME 타입을 결정
void serve_dynamic(int fd, char *filename, char *cgiargs); // 동적 콘텐츠(CGI)를 실행하고 결과를 전송
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg); // 오류 메시지를 HTML로 클라이언트에게 전송
void sigchld_handler(int sig); // 자식 프로세스가 종료될 때 좀비 프로세스를 제거하기 위한 시그널 핸들러

int main(int argc, char **argv)
{
  int listenfd, connfd; // 서버 리슨 소켓, 클라이언트 연결 소켓
  char hostname[MAXLINE], port[MAXLINE]; // 클라이언트 주소와 포트 저장용
  socklen_t clientlen; // 클라이언트 주소 길이
  struct sockaddr_storage clientaddr; // 클라이언트 주소 정보 저장 구조체

  if (argc != 2) { // 명령줄 인자 개수가 2가 아니면 (프로그램명 + 포트번호)
    fprintf(stderr, "usage: %s <port>\n", argv[0]); // 사용법 출력
    exit(1); // 프로그램 종료
  }

  Signal(SIGCHLD, sigchld_handler); // 자식 종료 시 좀비 프로세스 방지를 위한 시그널 핸들러 등록

  listenfd = Open_listenfd(argv[1]); // 포트 번호를 열어 서버 리슨 소켓 생성
  while (1) { // 무한 반복으로 클라이언트 요청 계속 처리
    clientlen = sizeof(clientaddr); // 주소 길이 초기화
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 클라이언트 연결 수락
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0); // 클라이언트 주소 정보 확인
    printf("Accepted connection from (%s, %s)\n", hostname, port); // 연결된 클라이언트 출력
    doit(connfd); // 요청 처리 함수 호출
    Close(connfd); // 연결 종료
  }
}

void doit(int fd)
{
  int is_static; // 정적 요청 여부
  struct stat sbuf; // 파일 상태 저장 구조체
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE]; // 요청 라인 저장용 버퍼
  char filename[MAXLINE], cgiargs[MAXLINE]; // 파일 경로 및 CGI 인자 저장용
  rio_t rio; // robust I/O 구조체
  int is_head = 0; // HEAD 요청 여부 확인용

  Rio_readinitb(&rio, fd); // rio 초기화
  Rio_readlineb(&rio, buf, MAXLINE); // 요청 라인 읽기
  printf("헤더 요청\n%s", buf); // 요청 라인 출력
  sscanf(buf, "%s %s %s", method, uri, version); // 요청 라인 파싱 (메서드, URI, 버전)

  if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "HEAD") != 0) {
    clienterror(fd, method, "501", "Not Implemented", "Tiny does not implement this method"); // 지원 안 되는 메서드는 에러 반환
    return;
  }

  if (strcasecmp(method, "HEAD") == 0)
    is_head = 1; // HEAD 요청이면 본문 생략 설정

  read_requesthdrs(&rio); // 헤더 읽기
  is_static = parse_uri(uri, filename, cgiargs); // URI 분석해서 정적/동적 판단

  if (stat(filename, &sbuf) < 0) { // 파일 존재 여부 확인
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find the file");
    return;
  }

  if (is_static) {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { // 읽기 가능한 정규 파일인지 확인
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size, is_head); // 정적 파일 제공
  } else {
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { // 실행 가능한 정규 파일인지 확인
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    if (!is_head)
      serve_dynamic(fd, filename, cgiargs); // 동적 컨텐츠 실행 및 전송
  }
}

void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE]; // 한 줄 저장용 버퍼
  Rio_readlineb(rp, buf, MAXLINE); // 첫 줄 읽기
  while (strcmp(buf, "\r\n")) { // 빈 줄 나올 때까지 반복
    Rio_readlineb(rp, buf, MAXLINE); // 다음 줄 읽기
    printf("%s", buf); // 출력
  }
}

int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr; // ? 위치 저장 포인터
  if (!strstr(uri, "cgi-bin")) { // 정적 콘텐츠 요청일 경우
    strcpy(cgiargs, ""); // CGI 인자 없음
    strcpy(filename, "."); // 현재 디렉토리 시작
    strcat(filename, uri); // 경로 연결
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html"); // 디폴트 파일 설정
    return 1; // 정적 요청
  } else {
    ptr = index(uri, '?'); // '?' 위치 찾기
    if (ptr) {
      strcpy(cgiargs, ptr + 1); // 인자 추출
      *ptr = '\0'; // '?' 제거
    } else
      strcpy(cgiargs, ""); // 인자 없음
    strcpy(filename, ".");
    strcat(filename, uri); // CGI 실행 파일 경로 설정
    return 0; // 동적 요청
  }
}

void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mpg"))
    strcpy(filetype, "video/mpeg");
  else
    strcpy(filetype, "text/plain"); // 기본 MIME
}

void serve_static(int fd, char *filename, int filesize, int is_head)
{
  int srcfd; // 소스 파일 디스크립터
  char *srcbuf; // 파일 내용 읽기용 버퍼
  char filetype[MAXLINE], buf[MAXBUF]; // MIME 타입, 응답 버퍼

  get_filetype(filename, filetype); // MIME 타입 확인
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf + strlen(buf), "Server: Tiny Web Server\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "Content-length: %d\r\n", filesize);
  sprintf(buf + strlen(buf), "Content-type: %s\r\n\r\n", filetype);
  Rio_writen(fd, buf, strlen(buf)); // 응답 헤더 전송

  if (is_head)
    return; // HEAD 요청은 본문 생략

  srcfd = Open(filename, O_RDONLY, 0); // 파일 열기
  srcbuf = (char *)Malloc(filesize); // 메모리 동적 할당
  Rio_readn(srcfd, srcbuf, filesize); // 파일 읽기
  Close(srcfd); // 파일 닫기
  Rio_writen(fd, srcbuf, filesize); // 본문 전송
  free(srcbuf); // 메모리 해제
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL}; // 빈 인자 리스트

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0) { // 자식 프로세스 생성
    setenv("QUERY_STRING", cgiargs, 1); // 환경 변수 설정
    Dup2(fd, STDOUT_FILENO); // 표준 출력 -> 소켓
    Execve(filename, emptylist, environ); // CGI 실행
  }
  Wait(NULL); // 부모는 자식 종료 대기
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF]; // 버퍼들

  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body + strlen(body), "<body bg color=\"ffffff\">\r\n");
  sprintf(body + strlen(body), "%s: %s\r\n", errnum, shortmsg);
  sprintf(body + strlen(body), "<p>%s: %s\r\n", longmsg, cause);
  sprintf(body + strlen(body), "<hr><em>The Tiny Web server</em>\r\n");

  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

void sigchld_handler(int sig)
{
  while (waitpid(-1, 0, WNOHANG) > 0) // 종료된 자식 reap
    ;
}