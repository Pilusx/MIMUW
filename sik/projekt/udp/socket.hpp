#ifndef UDP_SOCKET_HPP_
#define UDP_SOCKET_HPP_

#include "../include/base.hpp"
#include "../include/proto.hpp"
#include "join.hpp"
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace udp {

class VSocket {
protected:
#ifdef LOG_UDP
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

  int sock;
  struct sockaddr_in my_address;

  void assign_socket(uint32_t timeout);

  void close_socket();

public:
  explicit VSocket(uint32_t timeout) : logger("UDP", std::cout) {
    assign_socket(timeout);
  }

  VSocket(net::port_t port, uint32_t timeout) : logger("UDP", std::cout) {
    assign_socket(timeout);
    ucast::join(port, sock, my_address);
    logger.log("Successful ucast::join %1%:%2%",
               net::addr2str(my_address.sin_addr), port);
  }

  struct sockaddr_in get_address() {
    return my_address;
  }

  net::directed_buffer recv(bool nothrow = false);

  void recv_many(cmd::vdbuffer &buffers);

  void send(const cmd::directed_buffer &dbuffer);

  ~VSocket() { close_socket(); }
};

class MSocket : public VSocket {
private:
  struct ip_mreq ip_mreq;

public:
  MSocket(const char *multicast_dotted_address, net::port_t port,
          uint32_t timeout)
      : VSocket(timeout) {
    mcast::join(multicast_dotted_address, port, sock, ip_mreq, my_address);
    logger.log("Successful mcast::join on %1%:%2%", multicast_dotted_address,
               port);
  }

  template <typename T> void sendc(const T &msg) {
    cmd::directed_buffer dbuffer(my_address, msg);
    send(dbuffer);
  }

  ~MSocket() { mcast::quit(sock, ip_mreq); }
};
}; // namespace udp

#endif // UDP_SOCKET_HPP_
