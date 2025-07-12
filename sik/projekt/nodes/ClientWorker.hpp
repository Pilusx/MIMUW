#ifndef NODES_CLIENTWORKER_HPP_
#define NODES_CLIENTWORKER_HPP_

#include "../include/base.hpp"
#include "../include/fs.hpp"
#include "../include/proto.hpp"
#include "../include/tcp.hpp"
#include "../include/udp.hpp"
#include "CNode.hpp"

#include <boost/regex.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nodes {

class ClientWorker {
private:
#ifdef LOG_CLIENT
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

  std::unordered_map<char, std::function<void(std::string &)>> func;
  CNode<VUDPClient> unicast_node;
  CNode<MUDPClient> multicast_node;
  std::shared_ptr<fs::remote_index> manager;
  std::shared_ptr<fs::absolute_index> absolute_manager;

  struct ServerInfo {
    struct sockaddr_in srvr_addr;
    fs::fsize_t free_space;
  };

  std::vector<ServerInfo> servers;

  ServerInfo best_server();

  void discover(std::string &s);

  void search(std::string &s);

  void fetch(std::string &s);

  void remove(std::string &s);

  void upload(std::string &s);

  void other(std::string &s);

  void sync();

  void info();

  void exit(std::string &s);

public:
  ClientWorker(std::unique_ptr<VUDPClient> ucast_client,
               std::unique_ptr<MUDPClient> mcast_client,
               std::shared_ptr<fs::remote_index> manager,
               std::shared_ptr<fs::absolute_index> absolute_manager)
      : logger("Client", std::cout), unicast_node(std::move(ucast_client)),
        multicast_node(std::move(mcast_client)), manager(manager),
        absolute_manager(absolute_manager) {

    func['d'] = [&](std::string &s) { discover(s); };
    func['s'] = [&](std::string &s) { search(s); };
    func['f'] = [&](std::string &s) { fetch(s); };
    func['r'] = [&](std::string &s) { remove(s); };
    func['?'] = [&](std::string &s) { other(s); };
    func['e'] = [&](std::string &s) { exit(s); };
    func['u'] = [&](std::string &s) { upload(s); };

    info();
  }

  void dispatch(std::string &s) {
    char cmd = s[0];
    auto f = func.find(cmd);
    if (f != func.end()) {
      f->second(s);
    } else {
      throw ex::invalid_command(s);
    }
  }
};

} // namespace nodes

#endif // NODES_CLIENTWORKER_HPP_