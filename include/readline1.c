/*
 * 从字节流套接字读取一行文本.
 * 这个版本每次读取一个字节,遇到换行符则结束读取,由于频繁的系统调用read,
 * 效率低下.
 */
#include "unp.h"

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char c, *ptr;

  ptr = vptr;
  for (n = 1; n < maxlen; ++n) {
    if ((rc = read(fd, &c, 1)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    } else if (rc == 0) { /* EOF */
      *ptr = 0;
      return n - 1;
    } else {
      if (errno == EINTR)
        continue;
      return -1;
    }
  }
  *ptr = 0;
  return n;
}
