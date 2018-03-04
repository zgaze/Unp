/* ***********************************************************************

  > File Name: client.c
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Sun 04 Mar 2018 02:40:39 PM CST

 ********************************************************************** */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    printf("%s %s %s \n", argv[0], argv[1], argv[2]);
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
    
    char sendbuf[4096];
    char recvbuf[4096];
    while(fgets(sendbuf, 4096, stdin) != NULL) {
        if (sendbuf[strlen(sendbuf) - 1] == '\n')
            sendbuf[strlen(sendbuf) - 1] = '\0';
        sendto(sockfd, sendbuf, strlen(sendbuf), 0,
            (struct sockaddr*)&servaddr, sizeof(servaddr) );
        int n = recvfrom(sockfd, recvbuf, 4096, 0 ,NULL, NULL);
        recvbuf[n] = '\0';
        printf("%s\n", recvbuf);
    }
    return 0;
}
