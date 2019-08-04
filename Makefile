PREFIX=/usr/local

uacpid: uacpid.cc config.hh display.hh
	$(CXX) -std=c++17 -Wall -Wextra uacpid.cc -lX11 -lstdc++fs -pthread -o uacpid

install:
	@install -Dm755 -v uacpid $(PREFIX)/bin

clean:
	@rm -fv uacpid
