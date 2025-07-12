#include "join.hpp"

namespace mcast {
void join(const char *multicast_dotted_address, net::port_t local_port,
          int sock, struct ip_mreq &ip_mreq, struct sockaddr_in &my_address) {

  int optval;

  /* podpięcie się do grupy rozsyłania (ang. multicast) */
  ip_mreq.imr_multiaddr.s_addr = inet_addr(multicast_dotted_address);
  ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                 reinterpret_cast<void *>(&optval), sizeof optval) < 0)
    info::syserr<std::runtime_error>("setsockopt broadcast");

  optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    info::syserr<std::runtime_error>("setsockopt(SO_REUSEADDR) failed");

  optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) < 0)
    info::syserr<std::runtime_error>("setsockopt(SO_REUSEPORT) failed");

  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                 reinterpret_cast<void *>(&ip_mreq), sizeof ip_mreq) < 0)
    info::syserr<std::runtime_error>("setsockopt(IP_ADD_MEMBERSHIP) failed");

  /* podpięcie się pod lokalny adres i port */
  my_address.sin_family = AF_INET;
  my_address.sin_addr.s_addr = inet_addr(multicast_dotted_address);
  my_address.sin_port = htons(local_port);
  if (bind(sock, reinterpret_cast<struct sockaddr *>(&my_address),
           sizeof my_address) < 0)
    info::syserr<std::runtime_error>("bind");
}

void quit(int sock, struct ip_mreq &ip_mreq) {
  if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                 reinterpret_cast<void *>(&ip_mreq), sizeof ip_mreq) < 0)
    info::syserr<std::runtime_error>("setsockopt");
}

}; // namespace mcast

namespace ucast {
void join(net::port_t local_port, int sock, struct sockaddr_in &my_address) {
  int optval;

  // TODO wywalic ....
  optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    info::syserr<std::runtime_error>("setsockopt(SO_REUSEADDR) failed");

  optval = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) < 0)
    info::syserr<std::runtime_error>("setsockopt(SO_REUSEPORT) failed");

  /* podpięcie się pod lokalny adres i port */
  my_address.sin_family = AF_INET;
  my_address.sin_addr = net::get_external_address();
  my_address.sin_port = htons(local_port);
  if (bind(sock, reinterpret_cast<struct sockaddr *>(&my_address),
           sizeof my_address) < 0)
    info::syserr<std::runtime_error>("bind");
}
}; // namespace ucast