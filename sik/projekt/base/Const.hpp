#ifndef BASE_CONST_HPP_
#define BASE_CONST_HPP_

#include <cstdlib>

namespace net {

namespace udp {
constexpr size_t DEFAULT_DATA_SIZE = 1 << 8;
constexpr size_t MAX_PACKET_SIZE = 1 << 16;
} // namespace udp

namespace tcp {
constexpr size_t DEFAULT_DATA_SIZE = 1 << 14;
constexpr size_t MAX_PACKET_SIZE = 1 << 16;
} // namespace tcp

} // namespace net

#endif // BASE_CONST_HPP_