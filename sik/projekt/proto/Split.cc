#include "Split.hpp"

#include <algorithm>
#include <utility>

namespace cmd {

typedef std::string kebab;

void join(const std::vector<std::string> &names, cmd::kebab &kebab) {
  assert(names.size() > 0);
  kebab = names[0];
  for (auto it = ++names.begin(); it != names.end(); ++it) {
    kebab += " " + *it;
  }
}

void split_to_buffers(const cmd::kebab &kebab,
                      std::vector<std::string> &buffers) {
  size_t maxSize = net::udp::DEFAULT_DATA_SIZE;
  for (size_t pos = 0; pos < kebab.size(); pos += maxSize) {
    size_t Size = std::min(kebab.size() - pos, maxSize);
    buffers.push_back(kebab.substr(pos, Size));
  }
}

void encode(const std::vector<std::string> &names,
            std::vector<std::string> &buffers) {
  cmd::kebab kebab;
  join(names, kebab);
  split_to_buffers(kebab, buffers);
}

void split_to_names(const cmd::kebab &kebab, std::vector<std::string> &names) {
  size_t pos = 0;
  while (pos < kebab.size()) {
    size_t next = kebab.find_first_of(" ", pos);
    names.push_back(kebab.substr(pos, next - pos));
    if (next == std::string::npos)
      break;
    pos = next + 1;
  }
}

void decode(const cmd::vdbuffer &dbuffers, cmd::inv_mapnames &filenames) {
  std::map<sockaddr_in, cmd::kebab, cmd::sockaddr_cmp> kebabs;

  for (auto &dbuf : dbuffers) {
    cmd::simple simple = std::get<cmd::simple>(dbuf.get_buf());
    auto addr = dbuf.get_endpt();
    auto it = kebabs.find(addr);
    if (it != kebabs.end()) {
      it->second += simple.get_data();
    } else {
      kebabs[addr] = simple.get_data();
    }
  }

  for (auto &kebab : kebabs) {
    cmd::split_to_names(kebab.second, filenames[kebab.first]);
  }
}

void invert(const cmd::inv_mapnames &filenames, cmd::mapnames &mapnames) {
  for (auto &entry : filenames) {
    for (auto &filename : entry.second) {
      mapnames[filename] = entry.first;
    }
  }
}

} // namespace cmd

namespace net {
namespace tcp {
void split_nagle(net::buffer &nbuf, std::vector<fs::packet> &packets) {
  std::vector<char>::const_iterator it = nbuf.data.cbegin();

  while (it + 2 < nbuf.data.cend()) {

    uint16_t seq_no = 0;
    uint16_t len = 0;

    net::mem::readu16(it, seq_no);
    net::mem::readu16(it, len);
    if (it + len > nbuf.data.cend()) {
      it -= 2;
      break;
    }

    std::vector<char> data(len, 0);
    ::mem::read(it, data.data(), data.size());
    packets.push_back(fs::packet(seq_no, data));
    nbuf.pop_front(it - nbuf.data.cbegin());
    it = nbuf.data.cbegin();
  }
}
} // namespace tcp
} // namespace net