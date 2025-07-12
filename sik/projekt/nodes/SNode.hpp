#ifndef NODES_SNODE_HPP_
#define NODES_SNODE_HPP_

#include "ServerWorker.hpp"
#include <memory>
#include <utility>

namespace nodes {

template <typename UDPClient> class SNode {
private:
#ifdef LOG_UDP
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

  std::unique_ptr<UDPClient> client;
  std::shared_ptr<ServerWorker> worker;

public:
  SNode(std::unique_ptr<UDPClient> client, std::shared_ptr<ServerWorker> worker)
      : logger("LOG_UDP", std::cout), client(std::move(client)),
        worker(worker) {}

  void run() {
    cmd::vdbuffer responses;
    ssize_t len;

    for (;;) {
      do {
        net::directed_buffer nbuf = client->recv();
        len = nbuf.buf.len;

        if (nbuf.is_bad()) {
          info::syserr<std::runtime_error>(
              "error on datagram from client socket");
        } else {
          cmd::directed_buffer dbuf(nbuf);
          logger.log("Query %1%", dbuf);

          cmd::vdbuffer responses;
          worker->respond(dbuf, responses);

          for (cmd::directed_buffer &response : responses) {
            logger.log("Response %1%", response);
            client->send(response);
          }
        }
      } while (len > 0);

      logger.log("finished exchange");
    }
  }
};

} // namespace nodes

#endif // NODES_SNODE_HPP_