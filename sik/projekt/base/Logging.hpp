#ifndef BASE_LOGGING_HPP_
#define BASE_LOGGING_HPP_

#include <boost/format.hpp>
#include <cstdarg>
#include <cstddef>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <variant>
#include <vector>

#define UNUSED(...)

template <typename T> void io_prompt(std::string name, T &value) {
  std::cout << name << ": ";
  std::cin >> value;
}

namespace info {

template <typename... T> boost::format println(boost::format fmt, T... t) {
  return (fmt % ... % t);
}

template <typename... T> std::string vFormat(boost::format fmt, T... t) {
  return println(fmt, t...).str();
}

/* Wypisuje informację o błędnym zakończeniu funkcji systemowej
i rzuca wyjątek. */
template <typename EX, typename... T> void syserr(const char *fmt, T... t) {
  int errno1 = errno;
  std::string s = vFormat(boost::format(fmt), t...);
  std::string sf =
      vFormat(boost::format(" (%d; %s)\n"), errno1, strerror(errno1));
  throw EX(std::string(s + sf));
}

/* Wypisuje informację o błędzie i rzuca wyjątek. */
template <typename EX, typename... T> void fatal(const char *fmt, T... t) {
  throw EX(vFormat(boost::format(fmt), t...));
}

template <bool on> class Logger {
  std::string prefix;
  std::ostream &os;

public:
  Logger(std::string s, std::ostream &os) : prefix("[" + s + "] "), os(os) {}

  template <bool is_on = on, typename... T>
  std::enable_if_t<is_on, void> log(const char *fmt, T... t) {
    std::string pfmt = prefix + std::string(fmt);
    std::string msg = vFormat(boost::format(pfmt), t...);
    os << msg << std::endl;
  }

  template <bool is_on = on, typename... T>
  std::enable_if_t<!is_on, void> log(const char *, T...) {}
};

} // namespace info

namespace ex {
class named_exception : public std::exception {
protected:
  std::string s;

public:
  explicit named_exception(std::string s) : s(s) {}

  virtual const char *what() const noexcept { return s.c_str(); }
};

class tcp_error : public named_exception {
public:
  explicit tcp_error(std::string s)
      : named_exception("tcp runtime error: " + s) {}
};

class fs_error : public named_exception {
public:
  explicit fs_error(std::string s)
      : named_exception("fs runtime error: " + s) {}
};

class no_response : public named_exception {
public:
  no_response() : named_exception("No response to query") {}
};
} // namespace ex

#endif // BASE_LOGGING_HPP_