#include "socket.hpp"

namespace udp {
void VSocket::assign_socket(uint32_t timeout) {
  sock = socket(AF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
  if (sock < 0)
    info::syserr<std::runtime_error>("socket");

  net::set_sock_timeout(sock, timeout);
}

void VSocket::close_socket() {
  if (close(sock) == -1)
    info::syserr<std::runtime_error>("close");
}

net::directed_buffer VSocket::recv(bool nothrow) {
  int flags;
  struct sockaddr_in srvr_address;
  socklen_t rcva_len;
  net::buffer nbuf;
  ssize_t rcv_len;

  nbuf.data.clear();
  nbuf.data.resize(net::udp::MAX_PACKET_SIZE, 0);

  flags = 0;
  rcva_len = (socklen_t)sizeof(srvr_address);
  nbuf.len = rcv_len = recvfrom(sock, nbuf.data.data(), nbuf.data.size(), flags,
                                (struct sockaddr *)&srvr_address, &rcva_len);
  logger.log("Received %1% bytes", rcv_len);
  if (rcv_len >= 0) {
    nbuf.data.resize(rcv_len);
    logger.log("Bytes: %1%\n", nbuf);
  }

  if (!nothrow && rcv_len < 0) {
    info::syserr<std::runtime_error>("recvfrom");
  }

  return net::directed_buffer(srvr_address, nbuf);
}

void VSocket::recv_many(cmd::vdbuffer &buffers) {
  buffers.clear();
  do {
    net::directed_buffer b = recv(true);
    if (b.is_bad())
      break;

    buffers.push_back(cmd::directed_buffer(b));
  } while (true);
}

void VSocket::send(const cmd::directed_buffer &dbuffer) {
  int sflags;
  ssize_t snd_len;
  net::buffer nbuf = net::from_variant(dbuffer.get_buf());
  sockaddr_in endpt = dbuffer.get_endpt();
  socklen_t snda_len = sizeof(endpt);

  sflags = 0;
  snd_len = sendto(sock, nbuf.data.data(), nbuf.data.size(), sflags,
                   (struct sockaddr *)&endpt, snda_len);

  logger.log("Sent %1% bytes. Bytes: %2% \n", snd_len, nbuf);
  if (snd_len != nbuf.len)
    info::syserr<std::runtime_error>(
        "error on sending datagram to socket (%d != %d)", snd_len, nbuf.len);
}

}; // namespace udp