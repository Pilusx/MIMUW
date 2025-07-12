#include "server.hpp"
#include "../include/proto.hpp"
#include <vector>

namespace tcp {

void TCPServer::recv(FILE *file, fs::disk_space &space) {
  int msg_sock;

  struct sockaddr_in client_address;
  socklen_t client_address_len;

  ssize_t len;
  net::buffer tempbuf;

  for (;;) {
    client_address_len = sizeof(client_address);
    // get client connection from the socket
    msg_sock =
        accept(sock, (struct sockaddr *)&client_address, &client_address_len);

    net::buffer nbuf;
    if (msg_sock < 0)
      info::syserr<ex::tcp_error>("accept");
    do {
      len = tempbuf.read(msg_sock);

      logger.log("len=%1%", len);

      if (len < 0) {
        info::syserr<ex::tcp_error>("reading from client socket");
      } else if (len == 0) {
        logger.log("EOF\nExpected size matches=%1%", space.is_full());
      } else {
        nbuf.merge_with(tempbuf);
        std::vector<fs::packet> packets;
        logger.log("Splitting nagle packets");
        net::tcp::split_nagle(nbuf, packets);
        logger.log("Splitted into %1% packets.", packets.size());
        for (auto &fs_packet : packets) {
          logger.log("read from socket: packet.no = %1% -> %2% bytes:\n %3%",
                     fs_packet.get_seq(), fs_packet.get_len(),
                     fs_packet.get_buf());

          space.alloc(fs_packet.get_len());
          fs_packet.write(file);
        }
      }
    } while (len > 0);
    fflush(file);
    logger.log("ending connection");

    if (close(msg_sock) < 0)
      info::syserr<ex::tcp_error>("close");
    return;
  }
}

void TCPServer::send(FILE *file) {
  int msg_sock;

  struct sockaddr_in client_address;
  socklen_t client_address_len;

  client_address_len = sizeof(client_address);
  // get client connection from the socket
  msg_sock =
      accept(sock, (struct sockaddr *)&client_address, &client_address_len);
  if (msg_sock < 0)
    info::syserr<ex::tcp_error>("accept");

  uint16_t seq_no = 0;
  fs::packet fs_packet;
  while (fs_packet.read(file, seq_no++) > 0) {
    logger.log("sending %1% bytes : %2%", fs_packet.get_len(),
               fs_packet.get_buf());

    // we send numbers in network byte order
    net::buffer nbuf(fs_packet);
    nbuf.write(msg_sock);
  }

  if (close(msg_sock) < 0)
    info::syserr<ex::tcp_error>("close");
}

void Server::recv(std::promise<net::port_t> &port, const char *queue_length,
                  const char *dstfile, size_t file_size, int timeout) {
  FILE *file = fopen(dstfile, "w");
  fs::disk_space space;
  TCPServer server(port, queue_length, timeout);

  space.assign(file_size);
  server.recv(file, space);
  fclose(file);
}

void Server::send(std::promise<net::port_t> &port, const char *srcfile,
                  int timeout) {
  FILE *file = fopen(srcfile, "r");
  TCPServer server(port, "1", timeout);
  server.send(file);
  fclose(file);
}

} // namespace tcp