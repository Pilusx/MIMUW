#include "../include/proto.hpp"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <set>

std::string multiply(std::string s, size_t count) {
  std::string res;
  while (count--) {
    res += s + std::to_string(count);
  }
  return res;
}

size_t nullcount(std::string s) {
  return std::accumulate(s.begin(), s.end(), 0, [](size_t acc, char c) {
    return (c == 0 ? acc + 1 : acc);
  });
}

int main() {
  std::vector<std::string> filenames = {"ala",    multiply("ma", 150),   "kota",
                                        "a",      multiply("kebab", 40), "jest",
                                        "smaczny"};

  std::vector<cmd::simple> simples =
      cmd::server::creator::search(13, filenames);

  sockaddr_in addr;
  bzero(&addr, sizeof(sockaddr_in));
  cmd::vdbuffer dbuffers = cmd::server::helper::search(addr, simples);

  cmd::inv_mapnames inv_mapnames;
  cmd::decode(dbuffers, inv_mapnames);

  cmd::mapnames mapnames;
  cmd::invert(inv_mapnames, mapnames);

  std::vector<std::string> res;
  for (auto &name : mapnames) {
    res.push_back(name.first);
    // std::cout << name.first << std::endl;
  }

  if (filenames.size() != res.size()) {
    std::cerr << "Size do not match " << filenames.size() << " " << res.size()
              << std::endl;
    exit(1);
  }

  std::sort(filenames.begin(), filenames.end());
  std::sort(res.begin(), res.end());

  for (size_t pos = 0; pos < filenames.size(); ++pos) {
    bool b = res[pos].compare(filenames[pos]) != 0;
    if (b) {
      std::cerr << res[pos] << " != " << filenames[pos];
      exit(1);
    }
  }
}