#include "../include/tcp.hpp"
#include <iostream>
#include <string>

int main() {
  std::string port, filename;

  io_prompt("port", port);
  io_prompt("filename", filename);
  tcp::Server::send(port.c_str(), filename.c_str(), 3);
}