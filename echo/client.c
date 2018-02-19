/* ***********************************************************************

  > File Name: client.c
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Mon 19 Feb 2018 04:04:12 PM CST

 ********************************************************************** */

#include <stdio.h>
#include "../socket/socket.h"

#define MAXLINE 4096
void handle_echo(int connfd)
{
    char send_buf[MAXLINE], recv_buf[MAXLINE];
    while (fgets(send_buf, MAXLINE, stdin) != NULL) {
        writen(connfd, send_buf, strlen(send_buf));
        if (readline(connfd, recv_buf, MAXLINE) == 0)
            err_exit("disconnect");
        fputs(recv_buf, stdout);
    }
}


int main(int argc, const char* argv[])
{
    if (argc < 3)
        err_exit("usage: cli ip port");
    int sockfd = Tcp_connect(argv[1], (uint16_t)atoi(argv[2]));
    handle_echo(sockfd);
    return 0;
}

