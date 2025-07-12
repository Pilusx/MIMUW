#ifndef BASE_NET_HPP_
#define BASE_NET_HPP_

#include <arpa/inet.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <utility>
#include <vector>

std::ostream &operator<<(std::ostream &os, struct in_addr addr);

std::ostream &operator<<(std::ostream &os, struct sockaddr_in addr);

namespace rnd {

template <int N> struct rnd { void operator()(); };

template <> struct rnd<2> {
  uint16_t operator()() { return rand(); }
};

template <> struct rnd<4> {
  uint32_t operator()() { return rand(); }
};

template <> struct rnd<8> {
  uint64_t operator()() {
    uint64_t a = ((uint64_t)rand()) << 32;
    uint64_t b = rand();
    return a | b;
  }
};

} // namespace rnd

namespace net {

template <typename T> class Ring {
  T start, m;

public:
  Ring(T start, T m) : start(start), m(m) {
    srand(time(0) + std::hash<std::thread::id>()(std::this_thread::get_id()));
  }

  T assign() { return start + rnd::rnd<sizeof(T)>()() % m; }
};

typedef in_port_t port_t;

std::string addr2str(struct in_addr addr);

struct in_addr str2addr(const std::string &s);

void set_sock_timeout(int sock, uint32_t tv_sec);

in_addr get_external_address();

std::string normalize(std::string);

}; // namespace net

#endif // BASE_NET_HPP_
