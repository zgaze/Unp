#include <stdio.h>
#include "src/common/socket/socket.h"

#define MAXLINE 4096
void HandleEcho(int connfd) {
  char send_buf[MAXLINE], recv_buf[MAXLINE];
  while (fgets(send_buf, MAXLINE, stdin) != NULL) {
    writen(connfd, send_buf, strlen(send_buf));
    printf("send:%s", send_buf);
    if (readline(connfd, recv_buf, MAXLINE) == 0)
      err_exit("disconnect");
    fputs(recv_buf, stdout);
  }
}


int main(int argc, const char* argv[]) {
  if (argc < 3)
    err_exit("usage: cli ip port");
  int sockfd = TcpConnect(argv[1], (uint16_t)atoi(argv[2]));
  HandleEcho(sockfd);
  return 0;
}

