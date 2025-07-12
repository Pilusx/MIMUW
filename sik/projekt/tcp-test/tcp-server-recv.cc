#include "../include/tcp.hpp"
#include <iostream>
#include <string>

int main() {
  std::string port, filename;

  io_prompt("port", port);
  io_prompt("filename", filename);
  tcp::Server::recv(port.c_str(), "1", filename.c_str(), 1000, 3);
}