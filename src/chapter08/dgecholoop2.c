#include "unp.h"

/*
 * 减小UDP缓冲区大小测试丢包
 */
static void recvfrom_int(int);
static int count;

void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  int n;
  socklen_t len;
  char mesg[MAXLINE];

  Signal(SIGINT, recvfrom_int);

  n = 512;
  Setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

  for ( ; ; ) {
    len = clilen;
    Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

    ++count;
  }
}

static void
recvfrom_int(int signo)
{
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}
