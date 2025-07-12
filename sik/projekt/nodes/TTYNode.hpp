#ifndef NODES_TTYNODE_HPP_
#define NODES_TTYNODE_HPP_

#include "../include/base.hpp"
#include "ClientWorker.hpp"
#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace nodes {

class TTYNode {
protected:
#ifdef LOG_TTY
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;
  std::unique_ptr<ClientWorker> worker;

public:
  void operator()(std::string &s);

  explicit TTYNode(std::unique_ptr<ClientWorker> worker)
      : logger("LOG_TTY", std::cout), worker(std::move(worker)) {}

  bool parse(std::istream &is);

  bool run(std::istream &is) {
    for (;;) {
      parse(is);
    }
  }
};

} // namespace nodes

#endif // NODES_TTYNODE_HPP_