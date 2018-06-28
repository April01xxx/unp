/*
 * 在字节流套接字上调用read时读入的字节数可能比请求少,这并非出错状态.其原因是
 * 内核中套接字的缓冲区可能已经到达极限,此时需要再次调用read以读入剩余的字节
 */
#include "unp.h"

ssize_t
readn(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      /* read调用出错,如果是被中断则再次调用.某些系统会自动重启调用 */
      if (errno == EINTR)
        nread = 0;
      else
        return -1;
    } else if (nread == 0) { /* EOF */
      break;
    }
    nleft -= nread;
    ptr += nread;
  }
  return n-nleft;
}

ssize_t
Readn(int fd, void *ptr, size_t nbytes)
{
  ssize_t n;

  if ((n = readn(fd, ptr, nbytes)) < 0)
    err_sys("readn error");
  return n;
}
