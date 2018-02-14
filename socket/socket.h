/* ***********************************************************************

  > File Name: socket.h
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Wed 14 Feb 2018 04:56:47 PM CST

 ********************************************************************** */

#include "../unp.h"

ssize_t readn(int fd, void* vptr, size_t n)
{
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

ssize_t writen(int fd, const char* vptr, size_t n)
{
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



