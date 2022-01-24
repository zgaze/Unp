#include "src/common/socket/socket.h"
#include <pthread.h>
#include <signal.h>

void sig_child(int signal) {
  int stat;
  pid_t pid;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return ;
}

int max_fd = 0;
int fds[4096];
#define MAXLINE  4096
void* handle_all_fd() {
  int n = -1;
  char buf[MAXLINE];
  printf("Create a New Thread\n");
  while (1) {
    for (int i = 0; i < max_fd; ++i) {
      if (fds[i] == -1) continue;
      n = read(fds[i], buf, MAXLINE);
      if (n > 0) {
        printf("recive %d message:%s", n, buf);
        writen(fds[i], buf, n);
      } else if (n == 0) {
        printf("disconnect!\n");
        close(fds[i]);
        fds[i] = -1;
      } else if (n < 0) {
        // 不可读
      }
    }
    printf("all fd one loop done, sleep 1\n");
    sleep(1);
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
  pthread_create(&tid, NULL, &handle_all_fd, &connfd);
  memset(fds, 0, sizeof(fds));
  while (1) {
    if (-1 == (connfd = Accept(listen_fd)))
      handle_error("accept error");
    SetIoBlockability(connfd, 1);
    fds[max_fd] = connfd;
    __sync_fetch_and_add(&max_fd, 1);
  }
}

