#include "client.hpp"
#include "../include/proto.hpp"
#include <limits>
#include <vector>

namespace tcp {

void TCPClient::send(FILE *file) {
  uint16_t seq_no = 0;
  fs::packet fs_packet;
  net::buffer data_to_send;

  while (fs_packet.read(file, seq_no++) > 0) {
    logger.log("sending %1% bytes : %2%", fs_packet.get_len(),
               fs_packet.get_buf());

    // we send numbers in network byte order
    net::buffer nbuf(fs_packet);

    nbuf.write(sock);
  }
}

void TCPClient::recv(FILE *file, fs::disk_space &space) {
  net::buffer nbuf;
  ssize_t len;

  do {
    net::buffer tempbuf;
    len = tempbuf.read(sock);

    logger.log("len=%1%", len);

    if (len < 0) {
      info::syserr<ex::tcp_error>("reading from client socket");
    } else if (len == 0) {
      logger.log("EOF Expected size matches=%1%", space.is_full());
    } else {
      nbuf.merge_with(tempbuf);
      std::vector<fs::packet> packets;
      net::tcp::split_nagle(nbuf, packets);
      for (auto &fs_packet : packets) {
        logger.log("read from socket: packet.no = %1% -> %2% bytes:\n%3%",
                   fs_packet.get_seq(), fs_packet.get_len(),
                   fs_packet.get_buf());

        space.alloc(fs_packet.get_len());
        fs_packet.write(file);
      }
    }
  } while (len > 0);
  fflush(file);

  logger.log("ending connection");
}

void Client::recv(const char *host, const char *port, const char *dstfile,
                  int timeout) {
  FILE *file = fopen(dstfile, "w");
  fs::disk_space space;
  TCPClient client(host, port, timeout);

  space.assign(std::numeric_limits<fs::fsize_t>::max());
  client.recv(file, space);
  fclose(file);
}

void Client::send(const char *host, const char *port, const char *filename,
                  int timeout) {
  TCPClient client(host, port, timeout);
  FILE *file;

  file = fopen(filename, "r");
  client.send(file);
  fclose(file);
}
}; // namespace tcp
