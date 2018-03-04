/* ***********************************************************************

  > File Name: server.c
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Sun 04 Mar 2018 03:00:20 PM CST

 ********************************************************************** */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {                         
        printf("usage: %s ip port", argv[0]);
        exit(1);                             
    }    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;                    
    memset(&servaddr, 0, sizeof(servaddr));         
    servaddr.sin_family = AF_INET;                  
    servaddr.sin_port = htons(atoi(argv[2]));       
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
        socklen_t len = sizeof(servaddr);
    bind(sockfd, (struct sockaddr*)&servaddr, len);
    char buf[4096];
    struct sockaddr_in cliaddr;
    while (1) {
        socklen_t  clilen =  sizeof(cliaddr);
        int n = recvfrom(sockfd, buf, 4096, 0, 
            (struct sockaddr*)&cliaddr, &clilen);
        buf[n] = '\0';
        sendto(sockfd, buf, strlen(buf), 0, 
            (struct sockaddr*)&cliaddr, clilen);
    }
    return 0; 
}
