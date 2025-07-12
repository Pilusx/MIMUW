#ifndef PROTO_ENCODER_HPP_
#define PROTO_ENCODER_HPP_

#include "../include/base.hpp"
#include "Buffer.hpp"
#include <boost/regex.hpp>
#include <functional>
#include <string>
#include <vector>

namespace cmd {
/** Searches for a given command in the string **/
class Regex : public boost::regex {
public:
  explicit Regex(std::string s) : boost::regex(s) {}
  void validate(const std::string &cmd, boost::smatch &what);
  std::string mock() const { return this->str(); }
};

/** Searches for a given command with one string parameter **/
class Regex_String : public Regex {
private:
  std::string pref;

public:
  explicit Regex_String(std::string s);
  std::string mock(std::string s) const;
};

//** Searches for a given command with one path parameter **/
class Regex_Path : public Regex {
  std::string pref;

public:
  explicit Regex_Path(std::string s);
  std::string mock(std::string s) const;
};

/** Searches for a given command with one optional string parameter **/
class Regex_StringOptional : public Regex {
private:
  std::string pref;

public:
  explicit Regex_StringOptional(std::string s);
  std::string mock(std::string s = "") const;
};

template <typename T> void registerEncoder(std::string);

/** Translates message of given type into a sendable packet **/
template <typename T> class MessageEncoder : public std::string {
  static_assert(std::is_same<T, cmd::complex>::value ||
                    std::is_same<T, cmd::simple>::value,
                "Expected packet type");

public:
  explicit MessageEncoder(std::string s);

  /** Reads a message of type T from buffer. **/
  T read(const cmd::variant &buf) const;
  T read(const cmd::directed_buffer &dbuf) const;

  /** Filters all messages from src of given type T, and given msg type. **/
  void filter(const cmd::vdbuffer &src, cmd::vdbuffer &dst) const;
};

template class MessageEncoder<cmd::simple>;
template class MessageEncoder<cmd::complex>;

class SimpleEncoder : public MessageEncoder<cmd::simple> {
public:
  explicit SimpleEncoder(std::string cmd) : MessageEncoder(cmd) {}
  /** Returns a packet fullfilling the necessary conditions (cmd::simple) **/
  cmd::simple set(uint64_t cmd_seq, std::string data) const;
};

class ComplexEncoder : public MessageEncoder<cmd::complex> {
public:
  explicit ComplexEncoder(std::string cmd) : MessageEncoder(cmd) {}
  /** Returns a packet fullfilling the necessary conditions (cmd::complex) **/
  cmd::complex set(uint64_t cmd_seq, uint64_t param, std::string data) const;
};

/** Defines the form of client terminal requests. **/
namespace tty_parser {
extern Regex discover;
extern Regex_StringOptional search;
extern Regex_String fetch;
extern Regex_Path upload;
extern Regex_String remove;
extern Regex exit;
} // namespace tty_parser

/** Defines the form of client sendable queries. **/
namespace client {
namespace encoder {
extern SimpleEncoder discover;
extern SimpleEncoder search;
extern SimpleEncoder fetch;
extern SimpleEncoder remove;
extern ComplexEncoder upload;
} // namespace encoder

/** Parametrizes the client sendable message. **/
namespace creator {
cmd::simple discover();
cmd::simple search(std::string);
cmd::simple fetch(std::string);
cmd::simple remove(std::string);
cmd::complex upload(uint64_t, std::string);
} // namespace creator
} // namespace client

/** Defines the form of server sendable responses. **/
namespace server {
namespace encoder {
extern ComplexEncoder discover;
extern SimpleEncoder search;
extern ComplexEncoder fetch;
extern SimpleEncoder upload_NO;
extern ComplexEncoder upload_OK;
} // namespace encoder
namespace creator {
cmd::complex discover(uint64_t, fs::fsize_t, in_addr);
std::vector<cmd::simple> search(uint64_t, std::vector<std::string>);
cmd::complex fetch(uint64_t, net::port_t, std::string);
cmd::complex upload_OK(uint64_t, net::port_t);
cmd::simple upload_NO(uint64_t, std::string);
} // namespace creator
namespace helper {
std::vector<cmd::directed_buffer> search(sockaddr_in, std::vector<cmd::simple>);
} // namespace helper
} // namespace server

}; // namespace cmd

#endif // PROTO_ENCODER_HPP_
