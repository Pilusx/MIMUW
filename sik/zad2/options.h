#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <string>

typedef struct {
    std::string mcast_addr, name;
    int data_port, ctrl_port, psize, fsize, rtime;
} sender_options;

typedef struct {
    std::string discover_addr;
    int ctrl_port, ui_port, bsize, rtime;
} receiver_options;

sender_options sender_get_options(int argc, const char * const *argv);
receiver_options receiver_get_options(int argc, const char * const *argv);

#endif // _PARAMS_H_