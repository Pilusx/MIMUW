#include "Net.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

std::ostream &operator<<(std::ostream &os, struct in_addr addr) {
  os << inet_ntoa(addr);
  return os;
}

std::ostream &operator<<(std::ostream &os, struct sockaddr_in addr) {
  os << net::addr2str(addr.sin_addr) << ":" << ntohs(addr.sin_port);
  return os;
}

namespace net {
std::string addr2str(struct in_addr addr) {
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr), str, INET_ADDRSTRLEN);
  return std::string(str);
}

struct in_addr str2addr(const std::string &s) {
  struct in_addr addr;
  inet_aton(s.c_str(), &addr);
  return addr;
}

void set_sock_timeout(int sock, uint32_t tv_sec) {
  struct timeval tv;
  tv.tv_sec = tv_sec;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

in_addr get_external_address() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  assert(sock != -1);

  const char *DnsIp = "8.8.8.8";
  uint16_t kDnsPort = 53;
  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = inet_addr(DnsIp);
  serv.sin_port = htons(kDnsPort);

  int err =
      connect(sock, reinterpret_cast<const sockaddr *>(&serv), sizeof(serv));
  assert(err != -1);

  sockaddr_in name;
  socklen_t namelen = sizeof(name);
  err = getsockname(sock, reinterpret_cast<sockaddr *>(&name), &namelen);
  assert(err != -1);

  close(sock);
  return name.sin_addr;
}

std::string normalize(std::string s) { return std::string(s.c_str()); }

} // namespace net