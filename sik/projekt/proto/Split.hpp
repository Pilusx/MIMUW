/**
 * Functions used for dividing a group of filenames into packets.
 */

#ifndef PROTO_SPLIT_HPP_
#define PROTO_SPLIT_HPP_

#include "Buffer.hpp"
#include "Encoder.hpp"
#include <array>
#include <map>
#include <string>
#include <vector>

namespace cmd {

struct sockaddr_cmp {
  bool operator()(const sockaddr_in lhs, const sockaddr_in rhs) const {
    return lhs.sin_addr.s_addr < rhs.sin_addr.s_addr ||
           (lhs.sin_addr.s_addr == rhs.sin_addr.s_addr &&
            lhs.sin_port < rhs.sin_port);
  }
};

using inv_mapnames =
    std::map<sockaddr_in, std::vector<std::string>, sockaddr_cmp>;
using mapnames = std::map<std::string, sockaddr_in>;

void encode(const std::vector<std::string> &names,
            std::vector<std::string> &buffers);

void decode(const cmd::vdbuffer &dbuffers, cmd::inv_mapnames &filenames);

void invert(const cmd::inv_mapnames &filenames, cmd::mapnames &mapnames);

} // namespace cmd

namespace net {
namespace tcp {
/**
 *  Splits packets merged by the Nagle's algorithm
 */
void split_nagle(net::buffer &nbuf, std::vector<fs::packet> &packets);
} // namespace tcp
} // namespace net

#endif // PROTO_SPLIT_HPP_