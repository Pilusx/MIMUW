#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include <assert.h>
#include <future>
#include <inttypes.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/base.hpp"

namespace tcp {
class TCPServer {
  int sock;
  struct sockaddr_in server_address;

#ifdef LOG_TCP
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

public:
  TCPServer(std::promise<net::port_t> &port, const char *queue_length,
            int UNUSED(timeout))
      : logger("LOG_TCP_SERVER", std::cout) {

    sock = socket(PF_INET, SOCK_STREAM, 0); // creating IPv4 TCP socket
    if (sock < 0)
      info::syserr<ex::tcp_error>("socket");

    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htonl(0); // Get random port

    // bind the socket to a concrete address
    if (bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) <
        0) {
      info::fatal<ex::tcp_error>("random bind failed");
    }

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int len = sizeof(addr);
    getsockname(sock, reinterpret_cast<sockaddr *>(&addr),
                reinterpret_cast<socklen_t *>(&len));
    net::port_t assigned_port = ntohs(addr.sin_port);

    port.set_value(assigned_port);

    logger.log("Successful bind on port %1%", assigned_port);

    // switch to listening (passive open)
    if (listen(sock, atoi(queue_length)) < 0)
      info::syserr<ex::tcp_error>("listen");

    logger.log("accepting client connections on port %1%", assigned_port);
  }

  void recv(FILE *file, fs::disk_space &space);

  void send(FILE *file);
};

class Server {
public:
  static void recv(std::promise<net::port_t> &port, const char *queue_length,
                   const char *dstfile, size_t file_size, int timeout = 1);

  static void send(std::promise<net::port_t> &port, const char *srcfile,
                   int timeout);
};
}; // namespace tcp

#endif // TCP_SERVER_HPP_
