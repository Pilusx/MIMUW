#include "../include/nodes.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  std::string MCAST_ADDR, OUT_FLDR;
  uint32_t TIMEOUT;
  net::port_t CMD_PORT;
  fs::fsize_t MAX_SPACE;

  po::options_description desc{"Options"};
  desc.add_options()("help,h", "Help screen")(
      "MCAST_ADDR,g", po::value<std::string>(&MCAST_ADDR)->required(),
      "set the multicast address")(
      "CMD_PORT,p", po::value<net::port_t>(&CMD_PORT)->required(),
      "listen on this UDP port")(
      "MAX_SPACE,b",
      po::value<fs::fsize_t>(&MAX_SPACE)->default_value(52428800),
      "available space")("SHRD_FLDR,f",
                         po::value<std::string>(&OUT_FLDR)->required(),
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
      std::make_unique<MUDPClient>(MCAST_ADDR.c_str(), CMD_PORT, 0);

  std::unique_ptr<VUDPClient> ucast_client =
      std::make_unique<VUDPClient>(CMD_PORT, 0);

  std::shared_ptr<nodes::ServerWorker> server_worker =
      std::make_shared<nodes::ServerWorker>(
          OUT_FLDR.c_str(), MAX_SPACE, TIMEOUT, mcast_client->get_address());

  nodes::SNode<MUDPClient> mcast_node(std::move(mcast_client), server_worker);
  nodes::SNode<VUDPClient> ucast_node(std::move(ucast_client), server_worker);

  th::spawn([&]() { mcast_node.run(); });
  th::spawn([&]() { ucast_node.run(); });

  th::join();

  return 0;
}
