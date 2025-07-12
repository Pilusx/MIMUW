#ifndef TCP_CLIENT_HPP_
#define TCP_CLIENT_HPP_

#include <fstream>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/base.hpp"

namespace tcp {
class TCPClient {
  int sock;

#ifdef LOG_TCP
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

public:
  TCPClient(const char *host, const char *port, int UNUSED(timeout))
      : logger("LOG_TCP_CLIENT", std::cout) {
    int err;
    struct addrinfo addr_hints;
    struct addrinfo *addr_result;

    // 'converting' host/port in string to struct addrinfo
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    err = getaddrinfo(host, port, &addr_hints, &addr_result);
    logger.log("%1%:%2%", host, port);
    if (err == EAI_SYSTEM) { // system error
      info::syserr<ex::tcp_error>("getaddrinfo: %s", gai_strerror(err));
    } else if (err != 0) { // other error (host not found, etc.)
      info::fatal<ex::tcp_error>("getaddrinfo: %s", gai_strerror(err));
    }

    // initialize socket according to getaddrinfo results
    sock = socket(addr_result->ai_family, addr_result->ai_socktype,
                  addr_result->ai_protocol);
    if (sock < 0)
      info::syserr<ex::tcp_error>("socket");

    // connect socket to the server
    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen))
      info::syserr<ex::tcp_error>("connect");

    freeaddrinfo(addr_result);
  }

  void send(FILE *file);

  void recv(FILE *file, fs::disk_space &space);

  ~TCPClient() {
    if (close(sock) < 0) // socket would be closed anyway when the program ends
      info::syserr<ex::tcp_error>("close");
  }
};

class Client {
public:
  static void recv(const char *host, const char *port, const char *dstfile,
                   int timeout = 1);

  static void send(const char *host, const char *port, const char *filename,
                   int timeout);
};
}; // namespace tcp

#endif // TCP_CLIENT_HPP_
