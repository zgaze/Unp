#ifndef __SOCKET_H__
#define __SOCKET_H__
#include "src/common/err.h"

#include <sys/types.h>  
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>

void Test();

#define handle_error(msg) \
  do { perror(msg); exit(1); } while (0)

ssize_t readn(int fd, void* vptr, size_t n) {
  ssize_t  nread;
  size_t   nleft = n;
  char * ptr = (char*)vptr;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR){
        nread = 0;
      } else 
        return -1;
    } else if (nread == 0)
      break;
    nleft -= nread;
    ptr += nread;
  }
  return n - nleft;
}

ssize_t writen(int fd, const char* vptr, size_t n) {
  size_t nleft = n;
  ssize_t nwritten = 0;
  const char* ptr = vptr;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;  //write again
      else
        return -1;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}

ssize_t readline(int fd, void* vptr, size_t maxlen) {
  ssize_t  rc;
  char c, *ptr;
  ptr = (char*)vptr;
  size_t n = 1;
  for (; n < maxlen; n++) {
again:
    if ((rc = read(fd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    } else if (rc == 0) {
      *ptr = 0;
      return n - 1;
    } else {
      if (errno == EINTR)
        goto again;
      return -1;
    }
  }
  *ptr = 0;
  return n;
}


int Accept(int listen_fd) {
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  int connfd = accept(listen_fd, (struct sockaddr*)&cliaddr, &clilen);
ACCEPT_AGAIN:
  if (connfd == -1){
    printf("accept error\n");
    if (errno == EINTR || errno == EAGAIN)
      goto ACCEPT_AGAIN;
    else 
      return -1;
  } else
    return connfd;
}

int TcpListen(const char *ip, uint16_t port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
    handle_error("socket error");
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  if (ip == NULL)
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  else {
    if (inet_pton (AF_INET, ip, &servaddr.sin_addr) <= 0)
      handle_error("ip is wrong");
  }
  servaddr.sin_port = htons(port);
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
  if ( -1 == bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
    handle_error("bind error");
  if (-1 == listen(sockfd, 5000))
    handle_error("listen");

  return sockfd;
}

int TcpConnect(const char *ip, uint16_t port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
    handle_error("socket error");
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr)); 
  servaddr.sin_family = AF_INET;
  if (inet_pton (AF_INET, ip, &servaddr.sin_addr) <= 0)
    handle_error("ip is wrong");
  servaddr.sin_port = htons(port);
CONNECT_AGAIN:
  if (-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr) )) {
    if (errno == EINTR)
      goto CONNECT_AGAIN;
    else 
      handle_error("connect ");
  }
  return sockfd;
}

int SetIoBlockability(int fd, int nonblock) {
  int val;
  if (nonblock) {
    val = (O_NONBLOCK | fcntl(fd, F_GETFL));
  } else {
    val = (~O_NONBLOCK & fcntl(fd, F_GETFL));
  }
  return fcntl(fd, F_SETFL, val);
}


# endif // __SOCKET_H__
