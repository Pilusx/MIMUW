#include "Buffer.hpp"
#include <algorithm>

namespace net {

buffer::buffer(const cmd::simple &s) {
  data.clear();
  auto bins = std::back_inserter(data);
  std::string name = ::mem::bufString(s.cmd, sizeof(cmd::name_t));
  ::mem::write(bins, name.data(), name.length());
  net::mem::writeu64(bins, s.cmd_seq);
  ::mem::write(bins, s.data.data(), s.data.size());
  len = data.size();
}

buffer::buffer(const cmd::complex &c) {
  data.clear();
  auto bins = std::back_inserter(data);
  std::string name = ::mem::bufString(c.cmd, sizeof(cmd::name_t));
  ::mem::write(bins, name.data(), name.length());
  net::mem::writeu64(bins, c.cmd_seq);
  net::mem::writeu64(bins, c.param);
  ::mem::write(bins, c.data.data(), c.data.size());
  len = data.size();
}

buffer::buffer(const fs::packet &p) {
  data.clear();
  auto bins = std::back_inserter(data);
  net::mem::writeu16(bins, p.seq_no);
  net::mem::writeu16(bins, p.len);
  ::mem::write(bins, p.buffer.data(), p.buffer.size());
}

cmd::variant buffer::get() { return to_variant(*this); }

buffer from_variant(const ::cmd::variant &v) {
  buffer buf;
  std::visit([&](auto &&arg) { buf = net::buffer(arg); }, v);
  return buf;
}

ssize_t buffer::read(int sock, size_t MAX_SIZE) {
  data.clear();
  len = MAX_SIZE;
  data.resize(len, 0);
  len = ::read(sock, data.data(), data.size());

  if (len < 0) {
    data.resize(0);
  } else {
    data.resize(len);
  }
  return len;
}

ssize_t buffer::write(int sock) const {
  ssize_t len;
  if ((len = ::write(sock, data.data(), data.size())) != (ssize_t)data.size())
    info::syserr<ex::tcp_error>("partial / failed write");
  return len;
}

void buffer::merge_with(const buffer &buf) {
  auto bins = std::back_inserter(data);
  ::mem::write(bins, buf.data.data(), buf.data.size());
  len = data.size();
}

void buffer::pop_front(size_t size) {
  data.erase(data.begin(), data.begin() + size);
  len -= size;
  assert(len == (ssize_t)data.size());
}

std::ostream &operator<<(std::ostream &os, const net::buffer &buf) {
  os << std::string(buf.data.begin(), buf.data.end());
  return os;
}

} // namespace net

namespace fs {
size_t packet::size() const {
  return sizeof(seq_no) + sizeof(len) + buffer.size();
}

ssize_t packet::read(FILE *file, uint16_t seq_no, size_t MAX_SIZE) {
  this->seq_no = seq_no;
  buffer.clear();
  buffer.resize(MAX_SIZE, 0);
  ssize_t len = fread(buffer.data(), 1, buffer.size(), file);
  if (len < 0) {
    buffer.clear();
  } else {
    this->len = len;
    buffer.resize(len);
  }

  return len;
}

ssize_t packet::write(FILE *file) const {
  ssize_t len = fwrite(buffer.data(), 1, buffer.size(), file);
  if (len != (ssize_t)buffer.size())
    info::syserr<ex::tcp_error>("writing to file");

  return len;
}

} // namespace fs

namespace cmd {

void simple::udp_check() const { assert(size() <= net::udp::MAX_PACKET_SIZE); }

size_t simple::size() const {
  return sizeof(cmd::name_t) + sizeof(cmd::seq_t) + data.size();
}

size_t complex::size() const { return simple::size() + sizeof(param); }

simple::simple(std::string cmd, uint64_t cmd_seq, std::string data) {
  assert(cmd.length() <= sizeof(name_t));
  this->cmd = cmd;

  this->data = std::vector<char>(data.begin(), data.end());
  this->cmd_seq = cmd_seq;
  udp_check();
}

simple::simple(const net::buffer &buf) {
  std::vector<char>::const_iterator it = buf.data.begin();
  cmd = mem::readString(it, sizeof(cmd::name_t));
  net::mem::readu64(it, cmd_seq);
  data = std::vector<char>(it, buf.data.end());
}

complex::complex(const net::buffer &buf) {
  std::vector<char>::const_iterator it = buf.data.begin();
  cmd = mem::readString(it, sizeof(cmd::name_t));
  net::mem::readu64(it, cmd_seq);
  net::mem::readu64(it, param);
  data = std::vector<char>(it, buf.data.end());
}

cmd::seq_t directed_buffer::get_seq() const {
  cmd::seq_t res;
  std::visit([&](auto &&arg) { res = arg.get_seq(); }, buf);
  return res;
}

std::string directed_buffer::get_name() const {
  std::string res;
  std::visit([&](auto &&arg) { res = arg.get_name(); }, buf);
  return res;
}

std::ostream &operator<<(std::ostream &os, const cmd::simple &cmd) {
  os << "SIMPLE_CMD" << std::endl;
  os << "CMD     : " << cmd.get_name() << std::endl;
  os << "CMD_SEQ : " << cmd.get_seq() << std::endl;
  os << "DATA    : " << cmd.get_data() << std::endl;
  return os;
}

std::ostream &operator<<(std::ostream &os, const cmd::complex &cmd) {
  os << "CMPLX_CMD" << std::endl;
  os << "CMD     : " << cmd.get_name() << std::endl;
  os << "CMD_SEQ : " << cmd.get_seq() << std::endl;
  os << "PARAM   : " << cmd.get_param() << std::endl;
  os << "DATA    : " << cmd.get_data() << std::endl;
  return os;
}

std::ostream &operator<<(std::ostream &os, const cmd::directed_buffer &dbuf) {
  os << "DIRECTED_BUFFER" << std::endl;
  os << "ADDRESS : " << dbuf.get_endpt() << std::endl;
  std::visit([&](auto &&arg) { os << "with " << arg << std::endl; },
             dbuf.get_buf());
  return os;
}

}; // namespace cmd