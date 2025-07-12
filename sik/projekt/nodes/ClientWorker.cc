#include "ClientWorker.hpp"
#include <cctype>
#include <map>
#include <numeric>

namespace nodes {

void ClientWorker::discover(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::discover.validate(s, what);

  cmd::simple query = cmd::client::creator::discover();
  cmd::vdbuffer dbuffers;

  multicast_node.client->sendc(query);
  unicast_node.recv_many(cmd::server::encoder::discover, dbuffers,
                         query.get_seq(), 0);

  logger.log("Received %1%", dbuffers.size());
  servers.clear();
  for (auto &dbuffer : dbuffers) {
    cmd::complex complex = cmd::server::encoder::discover.read(dbuffer);

    servers.push_back({dbuffer.get_endpt(), complex.get_param()});
    struct in_addr mcast;
    std::string data = complex.get_data();
    inet_aton(data.c_str(), &mcast);
    info::discover(dbuffer.get_addr(), mcast, complex.get_param());
  }
  if (dbuffers.empty()) {
    logger.log("No available servers.");
  }
}

void ClientWorker::search(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::search.validate(s, what);
  std::string param = what[1];
  param.erase(0, 1);

  cmd::simple query = cmd::client::creator::search(param);
  cmd::vdbuffer dbuffers;
  std::vector<std::string> filenames;

  multicast_node.client->sendc(query);

  unicast_node.recv_many(cmd::server::encoder::search, dbuffers,
                         query.get_seq(), 0);

  cmd::inv_mapnames inv_names;
  cmd::mapnames mapnames;

  cmd::decode(dbuffers, inv_names);

  for (auto &entry : inv_names) {
    for (auto &filename : entry.second) {
      info::search(entry.first.sin_addr, filename);
    }
  }

  cmd::invert(inv_names, mapnames);
  manager->set_remote_files(mapnames);
}

void ClientWorker::fetch(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::fetch.validate(s, what);
  std::string filename = what[1];

  if (!manager->is_remote(filename)) {
    info::ignore("Not in remote files: " + filename);
    return;
  }

  cmd::simple query_simple = cmd::client::creator::fetch(filename);
  cmd::directed_buffer query(manager->whose_file(filename), query_simple);

  unicast_node.client->send(query);

  cmd::vdbuffer dbuffers;

  try {
    unicast_node.recv_many(cmd::server::encoder::fetch, dbuffers,
                           query_simple.get_seq());
  } catch (ex::named_exception &ex) {
    info::fetch(query.get_endpt(), false, filename, ex.what());
    return;
  }

  auto dbuf = dbuffers[0];
  cmd::complex complex = cmd::server::encoder::fetch.read(dbuf);

  std::unique_ptr<Transfer> transfer = std::make_unique<Transfer>();
  transfer->destination = net::addr2str(dbuf.get_addr());
  transfer->tcp_port = std::to_string(complex.get_param());
  transfer->filename = filename;

  fs::client::in_manager::recv(std::move(transfer), manager);
}

void ClientWorker::remove(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::remove.validate(s, what);
  std::string param = what[1];

  cmd::simple query = cmd::client::creator::remove(param);
  multicast_node.client->sendc(query);
}

typename ClientWorker::ServerInfo ClientWorker::best_server() {
  assert(!servers.empty());
  return std::accumulate(
      ++servers.begin(), servers.end(), servers[0], [](auto acc, auto elem) {
        return (acc.free_space < elem.free_space ? elem : acc);
      });
}

void ClientWorker::upload(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::upload.validate(s, what);
  std::string file_name = what[1];

  // Stage 0. Choose output file manager
  fs::bfs::path mock_path = file_name;
  std::shared_ptr<fs::virt::index> manager;

  if (mock_path.is_absolute()) {
    manager =
        std::reinterpret_pointer_cast<fs::virt::index>(this->absolute_manager);
  } else {
    manager = std::reinterpret_pointer_cast<fs::virt::index>(this->manager);
  }

  // Stage 1. Discover servers
  std::string mock_discover = cmd::tty_parser::discover.mock();
  discover(mock_discover);

  if (servers.empty()) {
    info::ignore("No available servers.");
    return;
  }

  // Stage 2. Get the best server and check if file can be uploaded.
  ServerInfo srvr = best_server();
  fs::fsize_t file_size = manager->get_file_size(file_name);

  if (!manager->is_available(file_name)) {
    info::upload(srvr.srvr_addr, info::UPLOAD_STATUS::NEXIST, file_name);
    return;
  }

  if (srvr.free_space < file_size) {
    info::upload(srvr.srvr_addr, info::UPLOAD_STATUS::TOOBIG, file_name);
    return;
  }

  // Stage 3. Send query to the chosen server.
  cmd::complex query = cmd::client::creator::upload(
      file_size, manager->get_file_name(file_name));
  cmd::directed_buffer dbuf(srvr.srvr_addr, query);

  unicast_node.client->send(dbuf);

  // Stage 4. Get response from server.
  cmd::vdbuffer dbuffers;
  unicast_node.recv_many(cmd::server::encoder::upload_OK, dbuffers,
                         query.get_seq(), 0);

  if (dbuffers.empty()) {
    info::ignore("No response from servers"); // No response.
    return;
  }

  // Stage 4.a Read the first message.
  auto &dbuffer = dbuffers[0];
  cmd::complex complex = std::get<cmd::complex>(dbuffer.get_buf());

  // Stage 4.b Ignore the rest.
  for (auto it = ++dbuffers.begin(); it != dbuffers.end(); ++it) {
    info::pckg_error(it->get_endpt(), "Too many responses for upload.");
  }

  // Stage 5. Send the file.
  std::unique_ptr<Transfer> transfer = std::make_unique<Transfer>();
  transfer->destination = net::addr2str(dbuffer.get_addr());
  transfer->tcp_port = std::to_string(complex.get_param());
  transfer->filename = file_name;

  fs::client::out_manager::send(std::move(transfer), manager);
}

void ClientWorker::other(std::string &s) {
  if (s == "?info") {
    info();
  } else if (s == "?sync") {
    sync();
  }
}

void ClientWorker::sync() {
  std::string mock_search = cmd::tty_parser::search.mock();
  search(mock_search);

  std::vector<std::string> filenames;
  manager->get_remote_files(filenames);

  for (auto filename : filenames) {
    if (!manager->is_available(filename)) {
      std::string mock_fetch = cmd::tty_parser::fetch.mock(filename);
      logger.log(mock_fetch.c_str());
      fetch(mock_fetch);
    }
  }
  info();
}

void ClientWorker::info() { logger.log("%1%", *manager); }

void ClientWorker::exit(std::string &s) {
  boost::smatch what;
  cmd::tty_parser::exit.validate(s, what);

  th::join();
  info();
  std::exit(0);
}

} // namespace nodes