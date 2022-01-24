#include "src/common/socket/socket.h"
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>

typedef struct {
    int fd;                  /* client's connection descriptor */
    struct sockaddr_in addr; /* client's address */
} CLIENT;

#define MAXLINE  4096
#define BUFF_SIZE 4096

int main(int argc, const char* argv[])
{
  if (argc < 3)
    err_exit("usage : %s ip port", argv[0]);
  int listen_fd = TcpListen(argv[1], (uint16_t)atoi(argv[2]));
  int i, maxi, maxfd, nready, nbytes, connfd;
  fd_set allset, rset;
  signal(SIGPIPE, SIG_IGN);
  char buffer[MAXLINE];
  CLIENT client[FD_SETSIZE]; /* FD_SETSIZE == 1024 */
  maxi = -1;
  maxfd = listen_fd;
  for (i = 0; i < FD_SETSIZE; i++) {
    client[i].fd = -1;
  }
  // 清空allset集合的标志位
  FD_ZERO(&allset);
  // 把监听socket放入这个集合中
  FD_SET(listen_fd, &allset);
  while (1) {
    rset = allset;
    // 定义两秒的超时时间
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    int nready = select(maxfd + 1, &rset, NULL, NULL, &timeout);
    if (nready < 0) {
      perror("select");
      break;
    } else if (nready == 0) { // 超时时间到了返回0
      printf("select time out\n");
      continue;
    }
    // 如果是监听socket的事件。说明是新链接
    if (FD_ISSET(listen_fd, &rset)) {
      struct sockaddr_in cliaddr;
      if (-1 == (connfd = AcceptWithAddr(listen_fd, &cliaddr)))
        handle_error("accept error");
      SetIoBlockability(connfd, 1);
      // 保存客户端连接的socket，放在之前定义的client数组中
      for (i = 0; i < FD_SETSIZE; i++) {
        if (client[i].fd < 0) {
          client[i].fd = connfd;
          client[i].addr = cliaddr;
          break;
        }
      }
      if (i == FD_SETSIZE)
        perror("too many clients");
      FD_SET(connfd, &allset);
      if (connfd > maxfd)
        maxfd = connfd; /* for select */
      if (i > maxi)
        maxi = i; /* max index in client[] array */
      if (--nready <= 0)
        continue; /* no more readable descriptors */
    }
    // 
    // 遍历所有的客户连接socket 处理已有连接可读的socket
    for (i = 0; i <= maxi; i++) {
      if ((connfd = client[i].fd) < 0)
        continue;
      // 依次检查每一个客户连接是否可读
      if (FD_ISSET(connfd, &rset)) {
        memset(buffer, 0, BUFF_SIZE);
        nbytes = recv(connfd, buffer, sizeof(buffer), 0);
        if (nbytes < 0) {
          perror("recv");
          continue;
        } else if (nbytes == 0) { // client 断开链接
          printf("Disconnect %d\n", connfd);
          FD_CLR(connfd, &allset);
          client[i].fd = -1;
        } else {
          printf("\nFrom %s:%d\n", inet_ntoa(client[i].addr.sin_addr),
            ntohs(client[i].addr.sin_port));
          printf("Recv: %sLength: %d\n", buffer, nbytes);
          writen(connfd, buffer, nbytes);
        }
        if (--nready <= 0)
          break;
      }
    }
  }
}

