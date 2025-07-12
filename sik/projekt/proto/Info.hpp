#ifndef PROTO_INFO_HPP_
#define PROTO_INFO_HPP_

#include "../include/base.hpp"
#include "Encoder.hpp"
#include <boost/format.hpp>
#include <initializer_list>
#include <ostream>
#include <string>

/** Loggable messages **/
namespace info {

void discover(struct in_addr server_addr, struct in_addr mcast_addr,
              uint32_t free_space, std::ostream &os = std::cout);

void search(struct in_addr server_addr, std::string file_name,
            std::ostream &os = std::cout);

void fetch(struct sockaddr_in server_addr, bool success, std::string file_name,
           std::string error, std::ostream &os = std::cout);

enum class UPLOAD_STATUS {
  NEXIST = 1,
  TOOBIG = 2,
  SUCCESS = 3,
  FAILED = 4,
};

void upload(struct sockaddr_in server_addr, UPLOAD_STATUS state,
            std::string file_name, std::string error = "",
            std::ostream &os = std::cout);

void pckg_error(struct sockaddr_in sender_addr, std::string error,
                std::ostream &os = std::cerr);

void ignore(std::string error, std::ostream &os = std::cout);

} // namespace info

#endif // PROTO_INFO_HPP_
