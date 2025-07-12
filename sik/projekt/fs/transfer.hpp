#ifndef FS_TRANSFER_HPP_
#define FS_TRANSFER_HPP_

#include "../include/base.hpp"
#include "../include/proto.hpp"
#include <string>

class Transfer {
public:
  std::string destination;
  std::string tcp_port;
  std::string filename;

  int timeout;

  sockaddr_in to_sockaddr() const {
    struct sockaddr_in addr;
    addr.sin_addr = net::str2addr(destination);
    addr.sin_port = ntohs(atoi(tcp_port.c_str()));
    return addr;
  }
};

#endif // FS_TRANSFER_HPP_