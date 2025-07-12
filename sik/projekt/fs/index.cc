#include "index.hpp"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <utility>

namespace fs {

void index::init() {
  std::lock_guard guard(*mutex);
  staged.clear();
  committed.clear();
  if (bfs::is_directory(directory)) {
    bfs::directory_iterator it(directory), eod;
    BOOST_FOREACH (bfs::path const &p, std::make_pair(it, eod)) {
      if (bfs::is_regular_file(p)) {
        index::tryWriteUnlocked(p, bfs::file_size(p));
        index::endWritingUnlocked(p);
      }
    }
  } else {
    bfs::create_directory(directory);
  }
}

void index::tryWriteUnlocked(const bfs::path &index, fsize_t file_size) {
  auto it = committed.find(index);
  if (it != committed.end()) {
    throw ex::fs_error((boost::format("File '%1%' already uploaded") %
                        index.filename().string())
                           .str());
  }

  it = staged.find(index);
  if (it != staged.end()) {
    throw ex::fs_error((boost::format("File '%1%' already staged for upload)") %
                        index.filename().string())
                           .str());
  }

  staged[index] = std::make_shared<file_state>(file_size);
  memory.alloc(file_size);
}

void index::endWritingUnlocked(const bfs::path &index) {
  file_ptr file;

  auto it = staged.find(index);
  assert(it != staged.end());
  file = it->second;
  staged.erase(it);

  if (file->size() > 0) {
    int diff = file->size() - bfs::file_size(index);
    assert(diff >= 0);
    if (diff > 0) {
      file->free(diff);
      memory.free(diff);
    }
  } else { // workaround for client fs ...
    file = std::make_shared<file_state>(bfs::file_size(index));
    memory.alloc(file->size());
  }
  committed[index] = file;
}

file_ptr index::tryRead(const bfs::path &index) {
  std::lock_guard guard(*mutex);
  auto it = committed.find(index);
  if (it != committed.end()) {
    return it->second;
  }
  return nullptr;
}

void index::tryRemove(const bfs::path &index) {
  std::lock_guard guard(*mutex);
  auto it = committed.find(index);
  if (it != committed.end()) {
    if (it->second.use_count() == 1) {
      committed.erase(it);
      bfs::remove(index);
    } else {
      ex::fs_error("File in use.");
    }
  }
  throw ex::fs_error("Not in committed.");
}

void index::tryWrite(const bfs::path &index, fsize_t file_size) {
  std::lock_guard guard(*mutex);
  tryWriteUnlocked(index, file_size);
}

void index::endWriting(const bfs::path &index) {
  std::lock_guard guard(*mutex);
  endWritingUnlocked(index);
}

fsize_t index::get_file_size(bfs::path &index) const {
  std::lock_guard guard(*mutex);
  auto it = committed.find(index);
  if (it == committed.end()) {
    return 0;
  }
  return it->second->size();
}

bool index::is_available(const bfs::path &index) const {
  std::lock_guard guard(*mutex);
  return committed.find(index) != committed.end();
}

bool index::is_staged(const bfs::path &index) const {
  std::lock_guard guard(*mutex);
  return staged.find(index) != staged.end();
}

bool index::is_indexed(const bfs::path &index) const {
  return is_available(index) && is_staged(index);
}

bfs::path index::get_file_path(const std::string &filename) const {
  return directory / filename;
}

fsize_t index::get_free_space() const { return memory.get_free_space(); }

} // namespace fs