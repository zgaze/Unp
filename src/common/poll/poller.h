#ifndef __POLLER_H__ 
#define __POLLER_H__ 

#include <sys/types.h>  
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <memory.h>
#include <map>
#include <string>
#include <vector>
#include "src/common/err.h"

struct RemoteInfo {
  uint16_t  remote_port; //远端端口
  std::string remote_ip; //远端 IP
  int64_t  last_tm; //上次交互的时间
};

struct ConnBufffer {
  std::string send_buffer;
  int32_t send_begin;
  int32_t send_end;
  std::string recv_buffer;
  int32_t recv_begin;
  int32_t recv_end;
};

// 每个fd上的信息
struct Fdinfo {
  uint32_t id;
  int sockfd;
  uint8_t type;
  uint8_t flag;
  ConnBufffer conn_buffer;
  RemoteInfo remote;
};

// epoll 循环
struct EventLoop {
  struct epoll_event* evs;
  std::map<int, Fdinfo> fds;
  std::vector<int> close_fds;
  std::vector<int> etin_fds;
  int epfd;
  int maxfd;
  int max_ev_num;
  int count;
};

#endif // __POLLER_H__
