#include "../include/nodes.hpp"
#include <boost/program_options.hpp>
#include <istream>
#include <limits>

namespace po = boost::program_options;

void graceful_exit(void) { th::join(); }

int main(int argc, char *argv[]) {
  std::string MCAST_ADDR, OUT_FLDR;
  uint32_t TIMEOUT;
  net::port_t CMD_PORT;

  po::options_description desc{"Options"};
  desc.add_options()("help,h", "Help screen")(
      "MCAST_ADDR,g", po::value<std::string>(&MCAST_ADDR)->required(),
      "set the multicast address")(
      "CMD_PORT,p", po::value<net::port_t>(&CMD_PORT)->required(),
      "send messages on this UDP port")(
      "OUT_FLDR,o", po::value<std::string>(&OUT_FLDR)->required(),
      "bfs::path to the file directory")(
      "TIMEOUT,t", po::value<uint32_t>(&TIMEOUT)->default_value(5), "timeout");

  po::variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << desc << '\n';
  } else { // Process arguments
    validate::interval_variable<uint32_t>(TIMEOUT, 1, 300, "TIMEOUT");
  }

  std::unique_ptr<MUDPClient> mcast_client =
      std::make_unique<MUDPClient>(MCAST_ADDR.c_str(), CMD_PORT, TIMEOUT);

  std::unique_ptr<VUDPClient> ucast_client =
      std::make_unique<VUDPClient>(CMD_PORT, TIMEOUT);

  std::shared_ptr<fs::remote_index> manager =
      std::make_shared<fs::remote_index>();

  std::shared_ptr<fs::absolute_index> absolute_manager =
      std::make_shared<fs::absolute_index>();

  manager->visit(OUT_FLDR, std::numeric_limits<fs::fsize_t>::max());
  manager->set_tcp_timeout(TIMEOUT);
  absolute_manager->set_tcp_timeout(TIMEOUT);

  std::unique_ptr<nodes::ClientWorker> client_worker =
      std::make_unique<nodes::ClientWorker>(std::move(ucast_client),
                                            std::move(mcast_client), manager,
                                            absolute_manager);

  nodes::TTYNode tty(std::move(client_worker));

  atexit(graceful_exit);

  tty.run(std::cin);
}