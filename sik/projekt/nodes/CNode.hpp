#ifndef NODES_CNODE_HPP_
#define NODES_CNODE_HPP_

#include "../include/base.hpp"
#include "../include/fs.hpp"
#include "../include/udp.hpp"
#include <memory>
#include <utility>

namespace nodes {

template <typename UDPClient> class CNode {
private:
#ifdef LOG_CNODE
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

public:
  std::unique_ptr<UDPClient> client;

  template <typename T>
  void recv_many(const cmd::MessageEncoder<T> &encoder, cmd::vdbuffer &res,
                 cmd::seq_t cmd_seq, size_t count = 1) {
    res.clear();
    cmd::vdbuffer dbuffers, dtemp;
    client->recv_many(dbuffers);

    // Filter by cmd_seq
    for (auto &dbuf : dbuffers) {
      uint64_t seq = dbuf.get_seq();
      if (seq != cmd_seq) {
        info::pckg_error(
            dbuf.get_endpt(),
            (boost::format("Expected other cmd_seq '%1%', got '%2%''%3%'.") %
             cmd_seq % seq % dbuf.get_name())
                .str());
      } else {
        dtemp.push_back(dbuf);
      }
    }

    // Filter by name
    encoder.filter(dtemp, res);

    if (res.size() < count) {
      info::fatal<ex::named_exception>("No response of type '%1%' cmd_seq '%2%",
                                       encoder.c_str(), cmd_seq);
    }

    logger.log("%1% out of %2% messages match the query \n", res.size(),
               dbuffers.size());
  }

  explicit CNode(std::unique_ptr<UDPClient> client)
      : logger("CNode", std::cout), client(std::move(client)) {}
};

} // namespace nodes

#endif // NODES_CNODE_HPP_