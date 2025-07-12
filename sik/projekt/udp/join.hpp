#ifndef UDP_JOIN_HPP_
#define UDP_JOIN_HPP_

#include "../include/base.hpp"

namespace mcast {
void join(const char *multicast_dotted_address, net::port_t local_port,
          int sock, struct ip_mreq &ip_mreq, struct sockaddr_in &my_address);

void quit(int sock, struct ip_mreq &ip_mreq);
}; // namespace mcast

#endif // UDP_JOIN_HPP_

namespace ucast {
void join(net::port_t local_port, int sock, struct sockaddr_in &my_address);
}