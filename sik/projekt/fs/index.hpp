#ifndef FS_INDEX_HPP_
#define FS_INDEX_HPP_

#include "../include/base.hpp"
#include <boost/filesystem.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace fs {
namespace bfs = boost::filesystem;

/**
 * File is removeable iff no one reads from it and no one is writing to it.
 * File is readable iff no one is writing to it,
 * File is writable iff it does not exist.
 */
class file_state {
private:
  fsize_t file_size;

public:
  explicit file_state(fsize_t size) : file_size(size) {}

  inline fsize_t size() { return file_size; }

  inline void free(fsize_t size) { file_size -= size; }
};

typedef std::shared_ptr<file_state> file_ptr;

namespace virt {
class index {
private:
  int tcp_timeout;

public:
  inline std::string get_file_name(const std::string &filename) const {
    return get_file_path(filename).filename().string();
  }
  virtual bfs::path get_file_path(const std::string &filename) const = 0;
  virtual fsize_t get_file_size(const std::string &filename) const = 0;
  virtual bool is_available(const std::string &filename) const = 0;
  int get_tcp_timeout() const { return tcp_timeout; }
  void set_tcp_timeout(int timeout) { tcp_timeout = timeout; }
};
} // namespace virt

class index : public virt::index {
protected:
  bfs::path directory;
  std::map<bfs::path, file_ptr> staged, committed;
  struct disk_space memory;
  std::shared_ptr<std::mutex> mutex;

  void tryWriteUnlocked(const bfs::path &index, fsize_t file_size);

  void endWritingUnlocked(const bfs::path &index);

  void init();

  file_ptr tryRead(const bfs::path &index);

  void tryRemove(const bfs::path &index);

  bool is_available(const bfs::path &index) const;

  bool is_staged(const bfs::path &index) const;

  bool is_indexed(const bfs::path &index) const;

  fsize_t get_file_size(bfs::path &index) const;

public:
  index() : mutex(std::make_shared<std::mutex>()) {}

  bfs::path get_file_path(const std::string &filename) const;

  // Called from new threads.
  void tryWrite(const bfs::path &index, fsize_t file_size);
  void endWriting(const bfs::path &index);

  fsize_t get_free_space() const;
};

} // namespace fs

#endif // FS_INDEX_HPP_