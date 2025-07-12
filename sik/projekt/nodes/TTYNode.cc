#include "TTYNode.hpp"
#include "../include/proto.hpp"
#include <string>

namespace nodes {

void TTYNode::operator()(std::string &s) {
  for (char &c : s) {
    if (c == ' ')
      break;
    c = tolower(c);
  }

  worker->dispatch(s);
}

bool TTYNode::parse(std::istream &is) {
  std::string line;
  if (!std::getline(is, line))
    return false;
  try {
    logger.log("%1%", line);
    this->operator()(line);
  } catch (ex::invalid_command &ex) {
    logger.log(ex.what());
  }
  return true;
}

} // namespace nodes