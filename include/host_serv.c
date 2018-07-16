#include "unp.h"

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME;  /* 返回CNAME,官方名字 */
  hints.ai_family = family;   /* AF_INET, AF_INET6, AF_UNSPEC, etc. */
  hints.ai_socktype = socktype; /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    return NULL;

  return res;
}

struct addrinfo *
Host_serv(const char *host, const char *serv, int family, int socktype)
{
  int n;
  struct addrinfo hints, *res;

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_CANONNAME;  /* 返回CNAME,官方名字 */
  hints.ai_family = family;   /* AF_INET, AF_INET6, AF_UNSPEC, etc. */
  hints.ai_socktype = socktype; /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    err_quit("host_serv error for %s, %s: %s",
        (host == NULL) ? "(no hostname)" : host,
        (serv == NULL) ? "(no service name)" : serv,
        gai_strerror(n));

  return res;
}
