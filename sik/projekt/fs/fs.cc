#include "fs.hpp"
#include "../include/base.hpp"
#include "../include/tcp.hpp"
#include <arpa/inet.h>

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <cassert>
#include <cstdlib>
#include <future>
#include <memory>

namespace fs {

void smart_index::visit(std::string &directory, fsize_t free_space) {
  memory.assign(free_space);
  this->directory = directory;
  index::init();
}

void smart_index::get_filenames(std::vector<std::string> &names,
                                std::string substr) const {
  for (auto f : committed) {
    std::string filename = f.first.filename().string();
    if (substr != "" && filename.find(substr) == std::string::npos) {
      continue;
    }
    names.push_back(filename);
  }
}

bool smart_index::is_available(const std::string &name) const {
  bfs::path index = get_file_path(name);
  return index::is_available(index);
}

bool smart_index::is_staged(const std::string &name) const {
  bfs::path index = get_file_path(name);
  return index::is_staged(index);
}

bool smart_index::is_indexed(const std::string &name) const {
  bfs::path index = get_file_path(name);
  return index::is_indexed(index);
}

fsize_t smart_index::get_file_size(const std::string &filename) const {
  bfs::path index = get_file_path(filename);
  return index::get_file_size(index);
}

void smart_index::remove(const std::string &file) {
  bfs::path index = get_file_path(file);
  index::tryRemove(index);
}

void remote_index::set_remote_files(std::map<std::string, sockaddr_in> &names) {
  remote_files.clear();
  for (auto it : names) {
    if (!is_indexed(it.first)) {
      remote_files.insert(it);
    }
  }
}

void remote_index::get_remote_files(std::vector<std::string> &names) const {
  for (auto it : remote_files) {
    names.push_back(it.first);
  }
}

void remote_index::remove_from_remote_files(std::string &name) {
  auto it = remote_files.find(name);
  if (it != remote_files.end()) {
    remote_files.erase(it);
  }
}

bool remote_index::is_remote(std::string &name) const {
  return remote_files.find(name) != remote_files.end();
}

sockaddr_in remote_index::whose_file(std::string name) {
  return remote_files[name];
}

namespace client {
void out_manager::send(std::unique_ptr<Transfer> transfer,
                       std::shared_ptr<fs::virt::index> manager) {
  bfs::path index = manager->get_file_path(transfer->filename);
  assert(manager->is_available(transfer->filename));
  transfer->timeout = manager->get_tcp_timeout();

  th::spawn(
      [](std::unique_ptr<Transfer> transfer, bfs::path index) {
        struct sockaddr_in addr = transfer->to_sockaddr();

        try {
          tcp::Client::send(transfer->destination.c_str(),
                            transfer->tcp_port.c_str(), index.string().c_str(),
                            transfer->timeout);
          info::upload(addr, info::UPLOAD_STATUS::SUCCESS, transfer->filename);
        } catch (std::exception &ex) {
          info::upload(addr, info::UPLOAD_STATUS::FAILED, transfer->filename,
                       ex.what());
        }
      },
      std::move(transfer), index);
}

void in_manager::recv(std::unique_ptr<Transfer> transfer,
                      std::shared_ptr<fs::remote_index> manager) {
  bfs::path index = manager->get_file_path(transfer->filename);
  transfer->timeout = manager->get_tcp_timeout();

  manager->tryWrite(index, 0);
  th::spawn(
      [](std::unique_ptr<Transfer> transfer, bfs::path index, auto manager) {
        struct sockaddr_in addr = transfer->to_sockaddr();
        try {
          tcp::Client::recv(transfer->destination.c_str(),
                            transfer->tcp_port.c_str(), index.string().c_str(),
                            transfer->timeout);

          manager->endWriting(index);
          manager->remove_from_remote_files(transfer->filename); // TODO hide

          info::fetch(addr, true, transfer->filename, "");
        } catch (std::exception &ex) {
          info::fetch(addr, false, transfer->filename, ex.what());
        }
      },
      std::move(transfer), index, manager);
}
} // namespace client
namespace server {
net::port_t file_manager::send(const std::string &filename) {
  std::promise<net::port_t> port;
  std::future<net::port_t> res = port.get_future();
  bfs::path index = get_file_path(filename);
  assert(is_available(filename));

  th::spawn(
      [](std::promise<net::port_t> &port, bfs::path index, int tcp_timeout) {
        tcp::Server::send(port, index.string().c_str(), tcp_timeout);
      },
      std::ref(port), index, get_tcp_timeout());
  res.wait();
  return res.get();
}

net::port_t file_manager::recv(const std::string &file, fs::fsize_t size) {
  std::promise<net::port_t> port;
  std::future<net::port_t> res = port.get_future();
  bfs::path index = get_file_path(file);

  tryWrite(index, size);
  th::spawn(
      [](std::promise<net::port_t> &port, bfs::path index, fsize_t size,
         file_manager *manager) {
        tcp::Server::recv(port, "1", index.string().c_str(), size, 3);
        manager->endWriting(index);
      },
      std::ref(port), index, size, this);
  res.wait();
  return res.get();
}

} // namespace server

boost::format file_fmt("%20s | %8.s | %6.s");

std::ostream &operator<<(std::ostream &os, const smart_index &index) {
  os << (boost::format("FILEMANAGER \n of directory %1% \n with %2% bytes of "
                       "available space \n") %
         index.directory % index.get_free_space())
     << std::endl;
  os << (file_fmt % "FILENAME" % "SIZE" % "STAGE") << std::endl;
  for (auto &file : index.committed)
    os << file_fmt % file.first.filename() % file.second->size() % "READABLE"
       << std::endl;

  for (auto &file : index.staged)
    os << file_fmt % file.first.filename() % file.second->size() % "LOADING"
       << std::endl;

  return os;
}

std::ostream &operator<<(std::ostream &os, const remote_index &index) {
  os << static_cast<const smart_index &>(index);
  for (const auto &file : index.remote_files)
    os << file_fmt % file.first % file.second % "REMOTE" << std::endl;
  return os;
}

} // namespace fs