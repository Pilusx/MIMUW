#include "ServerWorker.hpp"

namespace nodes {

void ServerWorker::dispatch(const cmd::directed_buffer &dbuffer,
                            cmd::vdbuffer &res) {
  std::string fname = dbuffer.get_name();

  logger.log("%1% of len %2%", fname, fname.size());

  auto it = fresponses.find(fname);
  if (it == fresponses.end()) {
    info::fatal<ex::invalid_command>(fname.c_str());
  }

  it->second(dbuffer, res);
}

void ServerWorker::respond(const cmd::directed_buffer &dbuffer,
                           cmd::vdbuffer &res) {
  std::lock_guard guard(mutex);
  res.clear();
  try {
    dispatch(dbuffer, res);
  } catch (ex::invalid_command &ex) {
    info::pckg_error(dbuffer.get_endpt(), ex.what());
  }
}

std::ostream &ServerWorker::operator<<(std::ostream &os) {
  os << "Responsive Server:" << std::endl;
  os << "Available CMD names: ";
  for (auto &fres : fresponses) {
    os << fres.first << " ";
  }
  os << std::endl;
  os << manager << std::endl;
  return os;
}

void ServerWorker::discover(const cmd::directed_buffer &dbuf,
                            cmd::vdbuffer &res) {
  cmd::simple in = cmd::client::encoder::discover.read(dbuf);

  cmd::complex ans = cmd::server::creator::discover(
      in.get_seq(), manager.get_free_space(), mcast_addr.sin_addr);

  res.push_back(cmd::directed_buffer(dbuf.get_endpt(), ans));
}

void ServerWorker::search(const cmd::directed_buffer &dbuf,
                          cmd::vdbuffer &res) {
  cmd::simple in = cmd::client::encoder::search.read(dbuf);

  std::string pattern = in.get_data();
  std::vector<std::string> filenames;

  manager.get_filenames(filenames, pattern);

  std::vector<cmd::simple> ans =
      cmd::server::creator::search(in.get_seq(), filenames);

  for (auto &dbuf : cmd::server::helper::search(dbuf.get_endpt(), ans)) {
    res.push_back(dbuf);
  }
}

inline net::port_t ServerWorker::fetch_async(std::string &filename) {
  return manager.send(filename);
}

void ServerWorker::fetch(const cmd::directed_buffer &dbuf, cmd::vdbuffer &res) {
  cmd::simple in = cmd::client::encoder::fetch.read(dbuf);
  std::string filename = in.get_data();

  if (manager.is_available(filename)) {
    try {
      net::port_t tcp_real = fetch_async(filename);

      cmd::complex ans =
          cmd::server::creator::fetch(in.get_seq(), tcp_real, filename);
      res.push_back(cmd::directed_buffer(dbuf.get_endpt(), ans));
    } catch (std::exception &ex) {
      info::pckg_error(dbuf.get_endpt(), ex.what());
    }
  } else {
    info::pckg_error(dbuf.get_endpt(), " File not available : " + filename);
  }
}

void ServerWorker::remove(const cmd::directed_buffer &dbuf,
                          cmd::vdbuffer &UNUSED(res)) {
  cmd::simple in = cmd::client::encoder::remove.read(dbuf);
  std::string filename = in.get_data();
  if (manager.is_available(filename)) {
    try {
      manager.remove(filename);
    } catch (std::exception &ex) {
      info::pckg_error(dbuf.get_endpt(), ex.what());
    }
  }
}

inline net::port_t ServerWorker::upload_async(std::string &filename,
                                              fs::fsize_t file_size) {
  return manager.recv(filename, file_size);
}

void ServerWorker::upload(const cmd::directed_buffer &dbuf,
                          cmd::vdbuffer &res) {
  cmd::complex in = cmd::client::encoder::upload.read(dbuf);
  std::string filename = in.get_data();
  auto size = in.get_param();
  auto free_space = manager.get_free_space();

  if (free_space < size || manager.is_available(filename)) {
    cmd::simple ans = cmd::server::creator::upload_NO(in.get_seq(), filename);
    res.push_back(cmd::directed_buffer(dbuf.get_endpt(), ans));
  } else {
    try {
      net::port_t tcp_real = upload_async(filename, size);
      cmd::complex ans =
          cmd::server::creator::upload_OK(in.get_seq(), tcp_real);
      res.push_back(cmd::directed_buffer(dbuf.get_endpt(), ans));
    } catch (std::exception &ex) {
      info::pckg_error(dbuf.get_endpt(), ex.what());
    }
  }
}
void ServerWorker::info() {
  std::stringstream ss;
  this->operator<<(ss);
  logger.log("%1%", ss.str());
}

} // namespace nodes
