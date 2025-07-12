#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <initializer_list>
#include <string>
#include <vector>
#include "err.h"

#define BUFFER_SIZE 2000
#define QUEUE_LENGTH 5

namespace Context {
// https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c
#define CLEAN_TERMINAL "\e[1;1H\e[2J"
#define ARROW_UP "\033\133\101"
#define ARROW_DOWN "\033\133\102"
#define SUBMIT "\r"

typedef int state_t;
typedef int field_t;

/* Field flags */
#define NONE 0
#define UPPER 1   // upper_bound field
#define LOWER 2   // lower_bound field
#define TRANS 4   // transition on submit
#define FINAL 8   // final field
#define PRESS 16  // pressed to display

typedef enum {
  Same = -1,
  OptionA,
  OptionB,
  End,
  OptionB1,
  OptionB2,
  Back
} fields_t;
typedef int field_t;

class Field {
 public:
  std::string displayed;
  std::string submitted;
  uint8_t flags;
  int next;

  Field(std::string displayed, uint8_t flags = NONE, int next = Same,
        std::string submitted = "")
      : displayed(displayed), flags(flags), next(next), submitted(submitted) {}
};

const Field menu_fields[] = {Field("Opcja A", UPPER | PRESS, Same, "A\n"),
                             Field("Opcja B", TRANS, OptionB1),
                             Field("Koniec", LOWER | FINAL, Same),
                             Field("Opcja B1", UPPER | PRESS, Same, "B1\n"),
                             Field("Opcja B2", NONE | PRESS, Same, "B2\n"),
                             Field("Wstecz", LOWER | TRANS, OptionA)};

class Menu {
  std::vector<field_t> fields;

 public:
  Menu(std::initializer_list<field_t> l) : fields(l) {}

  std::string to_string(state_t current) const {
    std::string str = CLEAN_TERMINAL;
    for (field_t f : fields)
      str += (f == current ? "->" : "  ") + menu_fields[f].displayed + "\n";

    return str;
  }
};

const Menu main_menu({OptionA, OptionB, End});
const Menu b_menu({OptionB1, OptionB2, Back});

class finished : public std::exception {
  virtual const char *what() const throw() { return "User finished."; }
};

class Automaton {
 private:
  state_t current;
  std::string suffix;

  void up() {
    if (!(menu_fields[current].flags & UPPER)) current--;
  }

  void down() {
    if (!(menu_fields[current].flags & LOWER)) current++;
  }

  void submit() {
    auto &field = menu_fields[current];
    if (field.flags & TRANS) {
      current = field.next;
      suffix = "";
    } else if (field.flags & FINAL) {
      throw finished();
    } else if (field.flags & PRESS) {
      suffix = field.submitted;
    }
  }

 public:
  Automaton() : current(OptionA), suffix("") {}
  void transition(std::string str) {
    // printf("TRANSITION %s\n", str.c_str());
    // for (char c : str) printf("%o ", c);
    // printf("\n");
    if (str.find(ARROW_UP) != std::string::npos) {
      up();
    } else if (str.find(ARROW_DOWN) != std::string::npos) {
      down();
    } else if (str.find(SUBMIT) != std::string::npos) {
      submit();
    }
  }

  operator std::string() {
    return (current <= End ? main_menu : b_menu).to_string(current) +
           (suffix != "" ? suffix : "");
  }
};

};  // namespace Context

namespace ErrorMessages {
#define err(x, msg) \
  if ((x) < 0) syserr(msg)

#define log(...) printf(__VA_ARGS__)
#define EUSAGE "usage: <prog_name> <port>"
#define EWRITE "writing to client socket"
#define EREAD "reading from client socket"
#define EPORT "wrong port %s"
#define MACCEPT "accepting client connections on port %hu\n"
#define MCONN "client connected\n"
#define MEND "ending connection\n"
#define MREAD "read from socket: %zd bytes: %.*s\n"

};  // namespace ErrorMessages

using namespace Context;
using namespace ErrorMessages;
#define MIN(x, y) ((x) < (y) ? (x) : (y))

class Server {
 private:
  int sock, msg_sock;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len;
  char buffer[BUFFER_SIZE];
  ssize_t len, snd_len;
  Automaton automaton;

  void send(const std::string &s) const {
    ssize_t len = 0, snd_len, expected_len;
    while (len != s.length()) {
      expected_len = MIN(BUFFER_SIZE, s.length() - len);
      snd_len = write(msg_sock, &s[len], expected_len);
      if (snd_len != expected_len) syserr(EWRITE);
      len += snd_len;
    }
  }

  void err_init() {
    err(sock = socket(PF_INET, SOCK_STREAM, 6), "socket");
    err(bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)),
        "bind");
    err(listen(sock, QUEUE_LENGTH), "listen");
  }

  void err_accept() {
    err(msg_sock = accept(sock, (struct sockaddr *)&client_address,
                          &client_address_len),
        "accept");
  }

  void err_close() { err(close(msg_sock), "close"); }

 public:
  Server(uint16_t port_num) : client_address_len(sizeof(client_address)) {
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_num);

    if (ntohs(server_address.sin_port) != port_num) syserr(EPORT, port_num);

    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  }

  void read_reply() {
    len = read(msg_sock, buffer, sizeof(buffer));
    if (len < 0)
      syserr(EREAD);
    else
      parse();
  }

  void parse() {
    // log(MREAD, len, (int)len, buffer);
    automaton.transition(buffer);
    send(automaton);
  }

  void init_client() {
    // IAC DO LINEMODE
    send("\377\375\042");
  }

  int run() {
    err_init();

    log(MACCEPT, ntohs(server_address.sin_port));
    for (;;) {
      automaton = Automaton();
      err_accept();

      log(MCONN);
      init_client();
      send(automaton);
      try {
        do {
          read_reply();
        } while (len > 0);
      } catch (finished f) {
      }
      log(MEND);
      err_close();
    }
  }
};

int main(int argc, char **argv) {
  if (argc != 2) syserr(EUSAGE);

  uint16_t port = atoi(argv[1]);
  if (std::to_string(port) != std::string(argv[1])) syserr(EPORT, argv[1]);
  return Server(port).run();
}
