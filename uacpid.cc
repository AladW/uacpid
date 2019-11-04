#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <system_error>
#include <vector>
#include <thread>

#include <pstreams/pstream.h>
#include <getopt.h>

typedef std::vector<std::string> argv_type;
typedef std::map<std::string, argv_type> stringmap;
typedef std::map<argv_type, argv_type> multi_stringmap;

#include "config.hh"
#include "display.hh"

template <typename T, typename U>
static bool exec_on_event(const T& event, const U& rules,
			  const std::string& str) {
  auto cmd = rules.find(event);
  bool cmd_status{};

  if (cmd != rules.end() && cmd->second.size()) {
    argv_type cmd_temp{cmd->second};
    if (cmd_temp.size() >= 2 && cmd_temp[1] == "<event>") {
      cmd_temp[1] = str;
    }

    redi::ipstream in(cmd_temp, redi::pstreams::pstderr);
    if (in.fail()) {
      fprintf(stderr, "WARNING: command %s failed to execute\n",
	      in.command().c_str());
    } else {
      cmd_status = true;
    }
  }
  return cmd_status;
}

static std::string get_active_tty() {
  std::ifstream sys{};
  sys.open("/sys/class/tty/tty0/active");

  if (sys.fail()) {
    fprintf(stderr, "WARNING: could not retrieve active terminal\n");
    return {};
  }

  std::string active_tty{};
  sys >> active_tty;
  return active_tty;
}

int main(int argc, char* argv[]) {
  static bool open_display{};
  static bool check_active_tty{};
  static std::string arg_tty{};

  static struct option opts[] = {
    {"help",        no_argument,       nullptr, 'h'},
    {"opendisplay", no_argument,       nullptr, 'o'},
    {"tty",         optional_argument, nullptr, 't'},
    {}
  };

  int opt{};
  while ((opt = getopt_long(argc, argv, "hot", opts, nullptr)) != -1) {
    switch (opt) {
    case 'h':
      fprintf(stderr, "usage: uacpid [-o|--opendisplay] [-t|--tty]\n");
      std::exit(0);
    case 'o':
      open_display = true;
      break;
    case 't':
      check_active_tty = true;
      if (optarg) {
	arg_tty = optarg;
      }
      break;
    default:
      std::exit(1);
    }
  }

  if (open_display) {
    if (XInitThreads()) {
      std::thread t(display_worker);
      t.detach();
    } else {
      throw std::invalid_argument("could not initialize Xlib threads");
    }
  }

  std::string start_tty{};
  if (check_active_tty) {
    if (arg_tty.size()) {
      start_tty = arg_tty;
    } else {
      start_tty = get_active_tty();
    }

    if (start_tty.size() == 0) {
      check_active_tty = false; // disable later TTY checks
    } else {
      fprintf(stderr, "NOTICE: started on active terminal %s\n", start_tty.c_str());
    }
  }

  redi::ipstream acpi_listen(ACPI_LISTEN_CMD, redi::pstreams::pstdout);
  if (acpi_listen.fail()) {
    fprintf(stderr, "ERROR: failed to execute acpi_listen\n");
    std::exit(1);
  }

  std::string line{};
  argv_type acpid_event(4);

  while (std::getline(acpi_listen, line)) {
    if (check_active_tty) {
      std::string active_tty = get_active_tty();

      if (active_tty.size() > 0 && active_tty != start_tty) {
	fprintf(stderr, "NOTICE: active and startup terminal differ, ignoring event\n");
	continue;
      }
    }

    std::stringstream ss(line);
    std::string arg{};
    for (size_t i = 0; i < 4; i++) {
      ss >> arg;
      acpid_event.at(i) = arg;
    }

    if (exec_on_event<argv_type, multi_stringmap>
	(acpid_event, rules, line)) {
      continue;
    }
    if (exec_on_event<std::string, stringmap>
	(acpid_event.at(0), rules_fallback, line)) {
      continue;
    }
  }
  acpi_listen.close();
  return 0;
}
