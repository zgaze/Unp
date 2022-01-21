/* ***********************************************************************

  > File Name: client.c
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Mon 19 Feb 2018 02:52:37 PM CST

 ********************************************************************** */

#include "src/common/socket/socket.h"
#include <signal.h>

void sig_child(int signal){
  int stat;
  pid_t pid;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return ;
}

#define MAXLINE  4096
void handle_echo(int connfd){
  ssize_t n;
  char buf[MAXLINE];
AGAIN:
  while ((n = read(connfd, buf, 4096)) > 0) {
    printf("[recive %d message:]\n%s", n, buf);
    // sleep(10);
    writen(connfd, buf, n);
  }
  if (n < 0 && errno == EINTR)
    goto AGAIN;
  else if (n < 0)
    err_exit("read error");
}

int main(int argc, const char* argv[])
{
  if (argc < 3)
    err_exit("usage : %s ip port", argv[0]);
  int listen_fd = TcpListen(argv[1], (uint16_t)atoi(argv[2]));
  int connfd;
  int childpid;
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, sig_child);
  while (1) {
    if (-1 == (connfd = Accept(listen_fd)))
      handle_error("accept error");
    handle_echo(connfd);
    close(connfd);
  }
}

