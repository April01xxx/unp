/*
 * 一个使用缓冲区的readline版本,较之前一个版本性能有所提升.
 * 一次读取期待读入MAXLINE个字节到缓冲区read_buf中,若其中还有内容
 * 则不调用read,否则调用read.
 */
#include "unp.h"

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{
  if (read_cnt <= 0) {
again:
    if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
      if (errno == EINTR)
        goto again;
      return -1;
    } else if (read_cnt == 0) { /* EOF */
      return 0;
    }
    read_ptr = read_buf;
  }
  --read_cnt;
  *ptr = *read_ptr++;
  return 1;
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
  ssize_t n, rc;
  char c, *ptr;

  ptr = vptr;
  for (n = 1; n < maxlen; ++n) {
    if ((rc = my_read(fd, &c)) == 1) {
      *ptr++ = c;
      if (c == '\n')
        break;
    } else if (rc == 0) {
      *ptr = 0;
      return n - 1;
    } else {
      return -1;
    }
  }
  *ptr = 0;
  return n;
}

/* 查看缓冲区的状态 */
ssize_t
readlinebuf(void **vptrptr)
{
  if (read_cnt)
    *vptrptr = read_ptr;
  return read_cnt;
}

ssize_t
Readline(int fd, void *ptr, size_t maxlen)
{
  ssize_t n;

  if ((n = readline(fd, ptr, maxlen)) < 0)
    err_sys("readline error");
  return n;
}
