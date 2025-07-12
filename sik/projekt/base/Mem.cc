#include "Mem.hpp"
#include "Net.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace mem {

std::string bufString(std::string src, size_t size) {
  assert(src.length() <= size);
  std::string res(size, 0);
  std::copy(src.begin(), src.end(), res.begin());
  assert(res.length() == size);
  return res;
}

void read(std::vector<char>::const_iterator &it, char *src, size_t size) {
  memcpy(src, &*it, size);
  it += size;
}

std::string readString(std::vector<char>::const_iterator &it, size_t size) {
  std::string res(size, 0);
  read(it, res.data(), size);
  return net::normalize(res);
}

void write(std::back_insert_iterator<std::vector<char>> &bins, const char *src,
           size_t size) {
  std::copy(src, src + size, bins);
}

} // namespace mem

namespace net {
namespace mem {
void readu16(std::vector<char>::const_iterator &it, uint16_t &host_value) {
  uint16_t net_value;
  ::mem::read(it, reinterpret_cast<char *>(&net_value), sizeof(net_value));
  host_value = ntohs(net_value);
}

void readu64(std::vector<char>::const_iterator &it, uint64_t &host_value) {
  uint64_t net_value;
  ::mem::read(it, reinterpret_cast<char *>(&net_value), sizeof(net_value));
  host_value = be64toh(net_value);
}

void writeu16(std::back_insert_iterator<std::vector<char>> &bins,
              const uint16_t &host_value) {
  uint16_t net_value = htons(host_value);
  ::mem::write(bins, reinterpret_cast<const char *>(&net_value),
               sizeof(net_value));
}

void writeu64(std::back_insert_iterator<std::vector<char>> &bins,
              const uint64_t &host_value) {
  uint64_t net_value = htobe64(host_value);
  ::mem::write(bins, reinterpret_cast<const char *>(&net_value),
               sizeof(net_value));
}
} // namespace mem
} // namespace net