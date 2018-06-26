/*
 * 在非阻塞的字节流套接字上调用write时写入的字节数可能比请求的少,这并非出错状态,
 * 其原因是内核的套接字缓冲区达到极限,此时需要再次调用write写入剩余的字节数.
 */
#include "unp.h"

ssize_t
writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  nleft = n;
  ptr = vptr;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0 && errno == EINTR)
        nwritten = 0;
      else
        return -1;
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}
