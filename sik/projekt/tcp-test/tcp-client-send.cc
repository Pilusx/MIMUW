#include "../include/tcp.hpp"
#include <iostream>
#include <string>

int main() {
  std::string host, port, filename;

  io_prompt("host", host);
  io_prompt("port", port);
  io_prompt("filename", filename);
  tcp::Client::send(host.c_str(), port.c_str(), filename.c_str(), 3);
}