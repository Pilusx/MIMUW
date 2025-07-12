#include "Validate.hpp"

namespace validate {
bool same_command(const std::string &cmd, boost::regex expr,
                  boost::smatch &what) {
  return boost::regex_search(cmd, what, expr);
}

void command(const std::string &cmd, boost::regex expr, boost::smatch &what) {
  if (!same_command(cmd, expr, what)) {
    throw ex::invalid_command(cmd);
  }
}
} // namespace validate