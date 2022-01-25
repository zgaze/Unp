#include "src/common/socket/socket.h"
#include "src/common/poll/poller.h"
#include <pthread.h>
#include <signal.h>

#define BUFF_SIZE 4096
int max_fd_num = 20000 ;

int main(int argc, const char* argv[])
{
  if (argc < 3)
    err_exit("usage : %s ip port", argv[0]);
  int listen_fd = TcpListen(argv[1], (uint16_t)atoi(argv[2]));
  int epfd = epoll_create(100);
  int connfd;
  int nbytes;
  char buffer[BUFF_SIZE];
  if (epfd < 0) {
    perror("epoll_create error");
    exit(5);
  }

  struct epoll_event ev, *events;
  ev.events = EPOLLIN;
  ev.data.fd = listen_fd;

  epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
  struct epoll_event ready_events[64];
  int timeout = -1;
  EventLoop loop;
  loop.evs = events;
  loop.epfd = epfd;
  while (1) {
    int nready = epoll_wait(epfd, ready_events, max_fd_num, timeout);
    int i = 0;
    for (; i < nready; ++i) {
      int ready_fd = ready_events[i].data.fd;
      // 如果是listen fd上的事件 说明是新连接
      if (listen_fd == ready_fd) {
        struct sockaddr_in cliaddr;
        if (-1 == (connfd = AcceptWithAddr(listen_fd, &cliaddr)))
          handle_error("accept error");
        SetIoNonBlock(connfd); // 将新连接置于非阻塞模式
        Fdinfo fd_info;
        fd_info.remote.remote_ip = inet_ntoa(cliaddr.sin_addr);
        fd_info.remote.remote_port = ntohs(cliaddr.sin_port);
        loop.fds[connfd] = fd_info;
        ev.events = EPOLLIN | EPOLLET; // 可读&ET模式
        ev.data.fd = connfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); // 新连接加到epoll
      } else if(ready_events[i].events & EPOLLIN) { //发来数据的连接
AGAIN:
        printf("EPOLLIN event\n");
        nbytes = read(ready_fd, buffer, BUFF_SIZE);
        // 处理读事件，包括正常的消息和close两种情况
        if (nbytes < 0) {
          perror("read failed");
          if (errno == EINTR) {
            goto AGAIN;
          }
          continue;
        } else if (nbytes == 0) {
          printf("Connect closed\n");
          epoll_ctl(epfd, EPOLL_CTL_DEL, ready_fd, NULL); //不再监视这个fd；
          // 从loop中删除
          Fdinfo fd_info;
          loop.fds[connfd] = fd_info;
          close(ready_fd);
        } else {
          buffer[nbytes] = '\0';
          printf("recive %d message:%s", nbytes, buffer);
          int nsend = writen(ready_fd, buffer, nbytes);
          // 如果当前写会被阻塞，监听可写事件
          if (nsend < nbytes) {
            ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
            ev.data.fd = ready_fd;
            epoll_ctl(epfd, EPOLL_CTL_MOD, ready_fd, &ev); //更改为epollout方式
            // 将剩余数据放入到发送缓冲区
            auto &fd_info = loop.fds[connfd];
            fd_info.conn_buffer.send_buffer.append(buffer + nsend, nbytes - nsend);
          }
        }
      } 
      if (ready_events[i].events & EPOLLOUT) { // 可写事件
        printf("EPOLLOUT event\n");
        // 从发送缓冲区里面读数据
        auto &fd_info = loop.fds[connfd];
        auto& buf = fd_info.conn_buffer.send_buffer;
        ssize_t nsend = writen(connfd, buf.data(), buf.size());
        if (nsend == buf.size()) {
          buf.clear();
          ev.events = EPOLLIN|EPOLLET;
          ev.data.fd = ready_fd;
          epoll_ctl(epfd, EPOLL_CTL_MOD, ready_fd, &ev); // 不再关注可写事件
        } else {
          buf = buf.substr(nsend, buf.size() - nsend);
        }
      }
    }
  }
  return 0;
}

