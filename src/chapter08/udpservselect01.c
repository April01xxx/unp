#include "unp.h"

int
main(int argc, char *argv[])
{
  int listenfd, connfd, udpfd, nready, maxfdp1;
  char mesg[MAXLINE];
  pid_t childpid;
  fd_set rset;
  ssize_t n;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddr;
  void sig_chld(int); /* chapter05 */

  /* 创建一个TCP监听套接字 */
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* 设置监听套接字的选项 */
  Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

  Listen(listenfd, LISTENQ);

  /* 创建一个UDP套接字 */
  udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  Bind(udpfd, (SA *)&servaddr, sizeof(servaddr));

  /* 安装信号处理函数 */
  Signal(SIGCHLD, sig_chld);

  FD_ZERO(&rset);
  maxfdp1 = max(listenfd, udpfd) + 1;
  for ( ; ; ) {
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);

    if ((nready = Select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
      if (errno = EINTR)
        continue;
      else
        err_sys("select error");
    }

    /* 处理就绪事件 */
    if (FD_ISSET(listenfd, &rset)) {
      len = sizeof(cliaddr);
      connfd = Accept(listenfd, (SA *)&cliaddr, &len);

      if ((childpid = Fork()) == 0) {
        Close(listenfd);
        str_echo(connfd);
        exit(0);
      }
      /* parent process should close connfd, otherwise the status of tcp
       * connection will be CLOSE_WAIT.
       */
      Close(connfd);
    }

    if (FD_ISSET(udpfd, &rset)) {
      len = sizeof(cliaddr);
      n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);

      Sendto(udpfd, mesg, n, 0, (SA *)&cliaddr, len);
    }
  }
}
