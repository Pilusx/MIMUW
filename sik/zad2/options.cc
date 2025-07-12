#include <boost/program_options.hpp>
#include <iostream>
#include "options.h"
#include "const.h"

namespace po = boost::program_options;

// https://www.boost.org/doc/libs/1_58_0/doc/html/program_options/tutorial.html#idp337609360
template<typename Options>
Options get_options(int argc, const char *const *argv, 
        void (*add_options)(po::options_description&, Options&)) {
    
    Options params;
    po::options_description desc("Allowed options");

    add_options(desc, params);

    desc.add_options()
        ("help", "help message");
    
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (std::exception e) {
        std::cout << desc << std::endl;
        exit(1);
    }
    
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(1);
    }

    return params;
}

void add_sender_options(po::options_description &desc, sender_options &opt) {
    desc.add_options()
        ("MCAST_ADDR,a", po::value<std::string>(&opt.mcast_addr)->default_value("255.255.255.255"), 
            "broadcast address")
        ("DATA_PORT,P", po::value<int>(&opt.data_port)->default_value(20000 + (student_id%10000)),
            "UDP port for data transmission")
        ("CTRL_PORT,C", po::value<int>(&opt.ctrl_port)->default_value(30000 + (student_id%10000)), 
            "UDP port for control packet transmission")
        ("PSIZE,p", po::value<int>(&opt.psize)->default_value(512), 
            "packet size in bytes")
        ("FSIZE,f", po::value<int>(&opt.fsize)->default_value(128 * 1024), 
            "FIFO queue size in bytes")
        ("RTIME,R", po::value<int>(&opt.rtime)->default_value(250), 
            "time interval between retransmissions in milliseconds")
        ("NAME,N", po::value<std::string>(&opt.name)->default_value("Nienazwany nadajnik"), 
            "name of the sender")
        ;
}

void add_receiver_options(po::options_description &desc, receiver_options &opt) {
    desc.add_options()
    ("DISCOVER_ADDR,d", po::value<std::string>(&opt.discover_addr)->default_value("255.255.255.255"),
        "detection address")
    ("CTRL_PORT,C", po::value<int>(&opt.ctrl_port)->default_value(30000+(student_id%10000)),
        "UDP port for control packet transmission")
    ("UI_PORT,U", po::value<int>(&opt.ui_port)->default_value(10000+(student_id%10000)),
        "TCP port for simple text UI")
    ("BSIZE,b", po::value<int>(&opt.bsize)->default_value(65536),
        "size of buffer")
    ("RTIME,R", po::value<int>(&opt.rtime)->default_value(250),
        "time interval between lost package raports")
    ;
}


sender_options sender_get_options(int argc, const char * const *argv) {
    return get_options<sender_options>(argc, argv, add_sender_options);
}
receiver_options receiver_get_options(int argc, const char * const *argv) {
    return get_options<receiver_options>(argc, argv, add_receiver_options);   
};