#include "Encoder.hpp"
#include "Split.hpp"
#include "Validate.hpp"
#include <algorithm>
#include <limits>
#include <set>
#include <string>

namespace cmd {

Regex_String::Regex_String(std::string s)
    : Regex(s + " ([,-9a-zA-Z]+)"), pref(s) {}

std::string Regex_String::mock(std::string s) const {
  return (boost::format("%1% %2%") % pref % s).str();
}

Regex_StringOptional::Regex_StringOptional(std::string s)
    : Regex(s + "( [,-9a-zA-Z]+)*"), pref(s) {}

std::string Regex_StringOptional::mock(std::string s) const {
  return (boost::format("%1% %2%") % pref % s).str();
}

Regex_Path::Regex_Path(std::string s)
    : Regex(s + " ([,-9a-zA-Z/]+)"), pref(s) {}

std::string Regex_Path::mock(std::string s) const {
  return (boost::format("%1% %2%") % pref % s).str();
}

namespace registered_encoders {
std::set<std::string> simpleEncoders;
std::set<std::string> complexEncoders;
} // namespace registered_encoders

template <> void registerEncoder<cmd::complex>(std::string name) {
  registered_encoders::complexEncoders.insert(name);
}

template <> void registerEncoder<cmd::simple>(std::string name) {
  registered_encoders::simpleEncoders.insert(name);
}

template <typename T>
MessageEncoder<T>::MessageEncoder(std::string s) : std::string(s) {
  registerEncoder<T>(s);
}

cmd::complex ComplexEncoder::set(uint64_t cmd_seq, uint64_t param,
                                 std::string data) const {
  return cmd::complex(*this, cmd_seq, param, data);
}

cmd::simple SimpleEncoder::set(uint64_t cmd_seq, std::string data) const {
  return cmd::simple(*this, cmd_seq, data);
}

template <typename T> T MessageEncoder<T>::read(const cmd::variant &buf) const {
  return std::get<T>(buf);
}

template <typename T>
T MessageEncoder<T>::read(const cmd::directed_buffer &dbuf) const {
  return read(dbuf.get_buf());
}

template <typename T>
void MessageEncoder<T>::filter(const cmd::vdbuffer &src,
                               cmd::vdbuffer &dst) const {
  std::copy_if(src.begin(), src.end(), std::back_inserter(dst),
               [&](auto &dbuf) {
                 cmd::variant var = dbuf.get_buf();

                 if (std::holds_alternative<T>(var)) {
                   std::string head = std::get<T>(var).get_name();
                   return head.find(*this) != std::string::npos;
                 } else {
                   return false;
                 }
               });
}

void Regex::validate(const std::string &cmd, boost::smatch &what) {
  validate::command(cmd, static_cast<boost::regex>(*this), what);
}

/** Defines the form of client terminal requests. **/
namespace tty_parser {
Regex discover("discover");
Regex_StringOptional search("search");
Regex_String fetch("fetch");
Regex_Path upload("upload");
Regex_String remove("remove");
Regex exit("exit");
} // namespace tty_parser

/** Defines the form of client sendable queries. **/
namespace client {
namespace encoder {
SimpleEncoder discover("HELLO");
SimpleEncoder search("LIST");
SimpleEncoder fetch("GET");
SimpleEncoder remove("DEL");
ComplexEncoder upload("ADD");
} // namespace encoder

/** Parametrizes the client sendable message. **/
namespace creator {
net::Ring<uint64_t> sequencer(std::numeric_limits<uint64_t>::min(),
                              std::numeric_limits<uint64_t>::max());

cmd::simple discover() { return encoder::discover.set(sequencer.assign(), ""); }

cmd::simple search(std::string pattern) {
  return encoder::search.set(sequencer.assign(), pattern);
}

cmd::simple fetch(std::string filename) {
  return encoder::fetch.set(sequencer.assign(), filename);
}

cmd::simple remove(std::string filename) {
  return encoder::remove.set(sequencer.assign(), filename);
}

cmd::complex upload(uint64_t file_size, std::string filename) {
  return encoder::upload.set(sequencer.assign(), file_size, filename);
}

} // namespace creator
} // namespace client

/** Defines the form of server sendable responses. **/
namespace server {
namespace encoder {
ComplexEncoder discover("GOOD_DAY");
SimpleEncoder search("MY_LIST");
ComplexEncoder fetch("CONNECT_ME");
SimpleEncoder upload_NO("NO_WAY");
ComplexEncoder upload_OK("CAN_ADD");
} // namespace encoder
namespace creator {
cmd::complex discover(uint64_t cmd_seq, fs::fsize_t free_space, in_addr addr) {
  return encoder::discover.set(cmd_seq, free_space, net::addr2str(addr));
}

std::vector<cmd::simple> search(uint64_t cmd_seq,
                                std::vector<std::string> names) {
  std::vector<std::string> buffers;
  std::vector<cmd::simple> res;

  cmd::encode(names, buffers);

  for (auto buf : buffers) {
    res.push_back(encoder::search.set(cmd_seq, buf));
  }
  return res;
}

cmd::complex fetch(uint64_t cmd_seq, net::port_t port, std::string name) {
  return encoder::fetch.set(cmd_seq, port, name);
}

cmd::complex upload_OK(uint64_t cmd_seq, net::port_t port) {
  return encoder::upload_OK.set(cmd_seq, port, "");
}

cmd::simple upload_NO(uint64_t cmd_seq, std::string name) {
  return encoder::upload_NO.set(cmd_seq, name);
}
} // namespace creator

namespace helper {
cmd::vdbuffer search(sockaddr_in addr, std::vector<cmd::simple> simples) {
  cmd::vdbuffer dbuffers;
  for (auto &s : simples) {
    cmd::directed_buffer dbuf(addr, s);
    dbuffers.push_back(dbuf);
  }
  return dbuffers;
}
} // namespace helper

} // namespace server

}; // namespace cmd

namespace net {
cmd::variant to_variant(const net::buffer &buf) {
  assert(buf.data.size() > sizeof(cmd::name_t));
  std::string name =
      net::normalize(std::string(buf.data.data(), sizeof(cmd::name_t)));

  const auto senc = cmd::registered_encoders::simpleEncoders;

  if (senc.find(name) != senc.end()) {
    return cmd::simple(buf);
  }

  const auto cenc = cmd::registered_encoders::complexEncoders;

  if (cenc.find(name) != cenc.end()) {
    return cmd::complex(buf);
  }

  throw ex::invalid_command(name);
}
} // namespace net