#ifndef NODES_SERVERWORKER_HPP_
#define NODES_SERVERWORKER_HPP_

#include "../include/base.hpp"
#include "../include/fs.hpp"
#include "../include/proto.hpp"
#include "../include/tcp.hpp"
#include "../include/udp.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace nodes {

class ServerWorker {
private:
  typedef std::function<void(const cmd::directed_buffer &, cmd::vdbuffer &)>
      Response;

  struct sockaddr_in mcast_addr;
  std::map<std::string, Response> fresponses;
  fs::server::file_manager manager;
  std::mutex mutex;

#ifdef LOG_SERVER
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

  void dispatch(const cmd::directed_buffer &buffer, cmd::vdbuffer &res);

  std::ostream &operator<<(std::ostream &os);

  void discover(const cmd::directed_buffer &buf, cmd::vdbuffer &res);

  void search(const cmd::directed_buffer &buf, cmd::vdbuffer &res);

  void fetch(const cmd::directed_buffer &buf, cmd::vdbuffer &res);

  void remove(const cmd::directed_buffer &buf, cmd::vdbuffer &UNUSED(res));

  void upload(const cmd::directed_buffer &buf, cmd::vdbuffer &res);

  net::port_t fetch_async(std::string &filename);

  net::port_t upload_async(std::string &filename, fs::fsize_t file_size);

  void info();

public:
  void respond(const cmd::directed_buffer &dbuffer, cmd::vdbuffer &res);

public:
#define fbind(qcmd, f)                                                         \
  fresponses[qcmd] = [&](const cmd::directed_buffer &buf,                      \
                         cmd::vdbuffer &res) { f(buf, res); };
  ServerWorker(const char *directory, fs::fsize_t free_space, uint32_t timeout,
               struct sockaddr_in mcast_addr)
      : mcast_addr(mcast_addr), logger("LOG_SERVER", std::cout) {
    std::string dirname = std::string(directory);
    manager.visit(dirname, free_space);
    manager.set_tcp_timeout(timeout);

    fbind(cmd::client::encoder::discover, discover);
    fbind(cmd::client::encoder::search, search);
    fbind(cmd::client::encoder::fetch, fetch);
    fbind(cmd::client::encoder::remove, remove);
    fbind(cmd::client::encoder::upload, upload);

    info();
  }
};

} // namespace nodes

#endif // NODES_SERVERWORKER_HPP_