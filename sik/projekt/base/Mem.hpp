#ifndef BASE_MEM_HPP_
#define BASE_MEM_HPP_

#include <string>
#include <vector>

namespace mem {
std::string bufString(std::string src, size_t size);

void read(std::vector<char>::const_iterator &it, char *src, size_t size);

std::string readString(std::vector<char>::const_iterator &it, size_t size);

void write(std::back_insert_iterator<std::vector<char>> &bins, const char *src,
           size_t size);
} // namespace mem

namespace net {
namespace mem {
void readu16(std::vector<char>::const_iterator &it, uint16_t &host_value);
void readu64(std::vector<char>::const_iterator &it, uint64_t &host_value);
void writeu16(std::back_insert_iterator<std::vector<char>> &bins,
              const uint16_t &host_value);
void writeu64(std::back_insert_iterator<std::vector<char>> &bins,
              const uint64_t &host_value);
} // namespace mem
} // namespace net

#endif // BASE_MEM_HPP_