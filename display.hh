#ifndef DISPLAY_H
#define DISPLAY_H
#include <X11/Xlib.h>

void display_worker() {
  // open connection to X display
  Display* display = XOpenDisplay(nullptr);

  if (display != nullptr) {
    fprintf(stderr, "NOTICE: opened screen %d\n", DefaultScreen(display));

    // block display on thread
    XLockDisplay(display);

    // ignore protocol errors
    XSetErrorHandler([](Display*, XErrorEvent* event) -> int {
	fprintf(stderr, "Ignoring Xlib error: error code %d request code %d\n",
		event->error_code, event->request_code);
	return 0;
      });

    // block on X event queue
    XEvent event{};
    while (XNextEvent(display, &event)) {
      continue;
    }

    // handle last event
    XCloseDisplay(display);
  } else {
    fprintf(stderr, "ERROR: could not open display\n");
    std::exit(1);
  }
}

#endif
