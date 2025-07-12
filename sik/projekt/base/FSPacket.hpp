#ifndef BASE_FSPACKET_HPP_
#define BASE_FSPACKET_HPP_

#include "Const.hpp"
#include "Logging.hpp"
#include <assert.h>
#include <mutex>
#include <sys/types.h>

namespace fs {
typedef size_t fsize_t;

/** Simulates the available space **/
class disk_space {
private:
  fsize_t free_space;

public:
  void assign(fsize_t mem_size) { this->free_space = mem_size; }

  void alloc(fsize_t file_size) {
    if (this->free_space < file_size) {
      info::fatal<std::runtime_error>(
          "Not enough memory (expected %1%, have %2%)", file_size,
          this->free_space);
    }
    this->free_space -= file_size;
  }

  void free(fsize_t file_size) { this->free_space += file_size; }

  inline fsize_t get_free_space() const { return free_space; }

  inline bool is_full() { return free_space == 0; }
};

} // namespace fs

#endif // BASE_FSPACKET_HPP_
