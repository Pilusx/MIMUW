#ifndef FS_FS_HPP_
#define FS_FS_HPP_

#include "index.hpp"

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "../include/base.hpp"
#include "../include/tcp.hpp"
#include "transfer.hpp"
#include <arpa/inet.h>

namespace fs {

class absolute_index : public virt::index {
protected:
  bool is_absolute_file(const bfs::path &index) const {
    return index.is_absolute() && bfs::is_regular_file(index);
  }

public:
  bfs::path get_file_path(const std::string &filename) const {
    bfs::path index = filename;
    assert(is_absolute_file(index));
    return index;
  }

  bool is_available(const std::string &filename) const {
    bfs::path index = get_file_path(filename);
    return is_absolute_file(filename);
  }

  size_t get_file_size(const std::string &filename) const {
    return bfs::file_size(get_file_path(filename));
  }
};

class smart_index : public index {
public:
  void visit(std::string &directory, fsize_t free_space);

  void get_filenames(std::vector<std::string> &names,
                     std::string substr = "") const;

  bool is_available(const std::string &name) const;

  bool is_staged(const std::string &name) const;

  bool is_indexed(const std::string &name) const;

  void remove(const std::string &file);

  fsize_t get_file_size(const std::string &filename) const;

  friend std::ostream &operator<<(std::ostream &os, const smart_index &index);
};

class remote_index : public smart_index {
protected:
  std::map<std::string, sockaddr_in> remote_files;

public:
  void set_remote_files(std::map<std::string, sockaddr_in> &names);

  void get_remote_files(std::vector<std::string> &names) const;

  void remove_from_remote_files(std::string &name);

  bool is_remote(std::string &name) const;

  sockaddr_in whose_file(std::string name);

  friend std::ostream &operator<<(std::ostream &os, const remote_index &index);
};

namespace client {
class out_manager {
public:
  static void send(std::unique_ptr<Transfer> transfer,
                   std::shared_ptr<fs::virt::index> manager);
};

class in_manager {
public:
  static void recv(std::unique_ptr<Transfer> transfer,
                   std::shared_ptr<fs::remote_index> manager);
};
} // namespace client

namespace server {
class file_manager : public fs::smart_index {
public:
  net::port_t send(const std::string &filename);
  net::port_t recv(const std::string &filename, fs::fsize_t size = 1000);
};
} // namespace server

/*
template <typename I>
std::ostream &operator<<(std::ostream &os, const fs::file_manager<I> &manager) {
  os << static_cast<const I &>(manager);
  os << boost::format("Manager has %1% seconds of tcp_timeout") %
            manager.tcp_timeout
     << std::endl;
  return os;
}
*/
} // namespace fs

#endif // FS_FS_HPP_
