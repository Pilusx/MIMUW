#include "Info.hpp"

namespace info {

void discover(struct in_addr server_addr, struct in_addr mcast_addr,
              uint32_t free_space, std::ostream &os) {
  os << boost::format("Found %1% (%2%) with free space %3%") % server_addr %
            mcast_addr % free_space
     << std::endl;
}

void search(struct in_addr server_addr, std::string file_name,
            std::ostream &os) {
  os << boost::format("%1% (%2%)") % file_name % server_addr << std::endl;
}

void fetch(struct sockaddr_in server_addr, bool success, std::string file_name,
           std::string error, std::ostream &os) {

  os << boost::format("File %1% %2%") % file_name %
            (success ? boost::format("downloaded (%1%)") % server_addr
                     : boost::format("downloading failed (%1%) %2%") %
                           server_addr % error)
                .str()
     << std::endl;
}

void upload(struct sockaddr_in server_addr, UPLOAD_STATUS state,
            std::string file_name, std::string error, std::ostream &os) {

  os << boost::format("File %1% %2%") % file_name %
            ([&]() -> boost::format {
              switch (state) {
              case UPLOAD_STATUS::NEXIST:
                return boost::format("does not exist");
              case UPLOAD_STATUS::TOOBIG:
                return boost::format("too big");
              case UPLOAD_STATUS::SUCCESS:
                return boost::format("uploaded (%1%)") % server_addr;
              case UPLOAD_STATUS::FAILED:
                return boost::format("uploading failed (%1%) %2%") %
                       server_addr % error;
              }
              return boost::format("");
            })()
                .str()
     << std::endl;
}

void pckg_error(struct sockaddr_in sender_addr, std::string error,
                std::ostream &os) {
  os << boost::format("[PCKG ERROR] Skipping invalid package from %1%. %2%") %
            sender_addr % error
     << std::endl;
}

void ignore(std::string error, std::ostream &os) {
  os << boost::format("Ignored. %1%") % error << std::endl;
}

} // namespace info