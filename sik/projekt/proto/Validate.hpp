#ifndef PROTO_VALIDATE_HPP_
#define PROTO_VALIDATE_HPP_

#include <boost/regex.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>

#include "../include/base.hpp"

template <typename T> std::string tostr(T value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <typename T> std::string tostr(const char *name, T value) {
  return std::string(name) + "=" + tostr(value);
}

template <typename T> std::string quoted(T val) {
  std::stringstream ss;
  static const std::string q = "\"";
  ss << q << val << q;
  return ss.str();
}

template <typename T> bool in_interval(T value, T lower_bound, T upper_bound) {
  return lower_bound <= value && value <= upper_bound;
}

namespace ex {
class invalid_command : public ex::named_exception {
public:
  explicit invalid_command(const std::string message)
      : named_exception("Unknown command: " + message) {}
};
} // namespace ex

namespace validate {
template <typename T>
void interval_variable(T value, T lower_bound, T upper_bound,
                       const char *name) {
  if (!in_interval(value, lower_bound, upper_bound)) {
    std::string msg = "variable " + tostr(name, value) +
                      " is not in the interval [" + tostr(lower_bound) + "," +
                      tostr(upper_bound) + "]";
    throw std::invalid_argument(msg);
  }
}

bool same_command(const std::string &cmd, boost::regex expr,
                  boost::smatch &what);

void command(const std::string &cmd, boost::regex expr, boost::smatch &what);
} // namespace validate

#endif // PROTO_VALIDATE_HPP_
