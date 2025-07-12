#ifndef PROTO_BUFFER_HPP_
#define PROTO_BUFFER_HPP_

#include "../include/base.hpp"
#include <cstdio>
#include <limits>
#include <list>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace cmd {
class simple;
class complex;
typedef std::variant<cmd::simple, cmd::complex> variant;
} // namespace cmd

namespace fs {
class packet;
} // namespace fs

namespace net {

/** Sendable buffer type **/
struct buffer {
  std::vector<char> data;
  ssize_t len;

  buffer() : len(0) { data.clear(); }

  explicit buffer(const cmd::simple &);
  explicit buffer(const cmd::complex &);
  explicit buffer(const fs::packet &);

  cmd::variant get();

  ssize_t read(int sock, size_t MAX_SIZE = net::tcp::MAX_PACKET_SIZE);
  ssize_t write(int sock) const;
  void merge_with(const buffer &buf);
  void pop_front(size_t size);

  friend std::ostream &operator<<(std::ostream &os, const buffer &buf);
};

struct directed_buffer {
  struct sockaddr_in addr;
  struct buffer buf;

  directed_buffer(sockaddr_in addr, net::buffer buf) : addr(addr), buf(buf) {}

  bool is_bad() const { return buf.len < 0; }
};

extern cmd::variant to_variant(const buffer &);

buffer from_variant(const cmd::variant &);

} // namespace net

namespace fs {
class packet {
private:
  uint16_t seq_no;
  uint16_t len;
  std::vector<char> buffer;

public:
  packet() : len(0) {}
  packet(uint16_t seq_no, std::vector<char> buffer)
      : seq_no(seq_no), len(buffer.size()), buffer(buffer) {
    assert(buffer.size() < std::numeric_limits<uint16_t>::max());
  }

  size_t size() const;

  inline uint16_t get_len() const { return len; }
  inline uint16_t get_seq() const { return seq_no; }
  inline std::string get_buf() const {
    return std::string(buffer.begin(), buffer.end());
  }

  ssize_t read(FILE *file, uint16_t seq_no,
               size_t MAX_SIZE = net::tcp::DEFAULT_DATA_SIZE);
  ssize_t write(FILE *file) const;
  void merge(const net::buffer &other);
  void pop_front(ssize_t size);

  friend class net::buffer;
};
} // namespace fs

namespace cmd {
typedef char name_t[10];
typedef std::vector<char> buffer_t;
typedef uint64_t seq_t;

/** Simple message type **/
class simple {
protected:
  std::string cmd;
  seq_t cmd_seq;
  buffer_t data;

  simple() = default;

  virtual size_t size() const;
  void udp_check() const;

public:
  simple(std::string cmd, uint64_t cmd_seq, std::string data);
  explicit simple(const net::buffer &);

  inline std::string get_name() const { return std::string(cmd); }
  inline std::string get_data() const {
    return std::string(data.begin(), data.end());
  }
  inline seq_t get_seq() const { return cmd_seq; }

  friend class net::buffer;
};

/** Complex message type **/
class complex : public simple {
protected:
  uint64_t param;

  size_t size() const;

public:
  complex(std::string cmd, uint64_t cmd_seq, uint64_t param, std::string data)
      : simple(cmd, cmd_seq, data) {
    this->param = param;
    simple::udp_check();
  }
  explicit complex(const net::buffer &);

  inline uint64_t get_param() const { return param; }

  friend class net::buffer;
};

/** Sendable buffer with a given receiver/sender.  **/
class directed_buffer {
  struct sockaddr_in addr;
  cmd::variant buf;

public:
  directed_buffer(sockaddr_in addr, cmd::variant buf) : addr(addr), buf(buf) {}
  explicit directed_buffer(net::directed_buffer nbuf)
      : addr(nbuf.addr), buf(nbuf.buf.get()) {}

  inline net::port_t get_port() const { return addr.sin_port; }
  inline in_addr get_addr() const { return addr.sin_addr; }
  inline sockaddr_in get_endpt() const { return addr; }
  inline cmd::variant get_buf() const { return buf; }
  cmd::seq_t get_seq() const;
  std::string get_name() const;

  friend std::ostream &operator<<(std::ostream &os,
                                  const cmd::directed_buffer &dbuf);
};

/** Group of sendable buffers. **/
typedef std::vector<directed_buffer> vdbuffer;

} // namespace cmd

#endif // PROTO_BUFFER_HPP_
