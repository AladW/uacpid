PREFIX ?= $(HOME)/.local

uacpid: uacpid.cc config.hh display.hh
	$(CXX) -std=c++17 -Wall -Wextra uacpid.cc -lX11 -pthread -o uacpid

install:
	@install -Dm755 -v uacpid $(PREFIX)/bin

uninstall:
	@rm -v $(PREFIX)/bin/uacpid

reload: uacpid install
	@systemctl --user restart uacpid.service
	@systemctl --user status  uacpid.service

clean:
	@rm -fv uacpid
