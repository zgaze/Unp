#include "src/common/socket/socket.h"
#include <signal.h>

void sig_child(int signal) {
  int stat;
  pid_t pid;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return ;
}

#define MAXLINE  4096
void* handle_echo(void* fd) {
  printf("Create a New Thread\n");
  int connfd = *(int*)fd;
  ssize_t n;
  char buf[MAXLINE];
  while (1) {
    n = read(connfd, buf, 4096);
    printf("read return %d\n", n);
    if (n > 0) {
      printf("recive %d message:%s", n, buf);
      writen(connfd, buf, n);
      continue;
    } else if (n == 0) {
      printf("disconnect!\n");
      close(connfd);
      break;
    } else if (n < 0 && errno == EINTR) {
      continue;
    } else if (n < 0) {
      printf("read wait loop once\n");
      sleep(1);
    }
  }
  return NULL;
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
  pthread_t tid;
  while (1) {
    if (-1 == (connfd = Accept(listen_fd)))
      handle_error("accept error");
    SetIoBlockability(connfd, 1);
    pthread_create(&tid, NULL, &handle_echo, &connfd);
  }
}

