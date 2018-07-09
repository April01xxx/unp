#include "unp.h"

int
main(int argc, char *argv[])
{
  int i, maxi, maxfd, listenfd, connfd, sockfd;
  int nready, client[FD_SETSIZE];
  ssize_t n;
  fd_set rset, allset;
  char buf[MAXLINE];
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;

  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

  Listen(listenfd, LISTENQ);

  maxfd = listenfd;
  maxi = -1;
  for (i = 0; i < FD_SETSIZE; ++i)
    client[i] = -1;
  FD_ZERO(&allset);
  FD_SET(listenfd, &allset);

  /* 循环处理请求 */
  for ( ; ; ) {
    rset = allset;
    nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

    if (FD_ISSET(listenfd, &rset)) {
      /* 新的连接 */
      clilen = sizeof(cliaddr);
      connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

      /* 将新的sockfd放入client数组中 */
      for (i = 0; i < FD_SETSIZE; ++i)
        if (client[i] < 0) {
          client[i] = connfd;
          break;
        }
      if (i == FD_SETSIZE)
        err_quit("too many clients");

      FD_SET(connfd, &allset);
      if (connfd > maxfd)
        maxfd = connfd;
      if (i > maxi)
        maxi = i;

      if (--nready <= 0)  /* 所有准备就绪的事件处理完成 */
        continue;
    }

    for (i = 0; i <= maxi; ++i) {
      if ((sockfd = client[i]) < 0)
        continue;
      if (FD_ISSET(sockfd, &rset)) {
        /* 该sockfd可读 */
        if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
          /* 客户端关闭连接 */
          Close(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        } else {
          Writen(sockfd, buf, n);
        } /* Read函数已经处理read返回值小于0的情况 */

        if (--nready <= 0)
          break;
      }
    }
  }
}
