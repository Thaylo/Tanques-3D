/**
 * joystick.cpp - Joystick input handling implementation
 */

#include "rendering/joystick.h"
#include <cstring>

#ifdef __linux__
#include <linux/joystick.h>
#endif

Joystick::Joystick() : fd(-1), connected(false), axisX(0), axisY(0) {
  memset(buttons, 0, sizeof(buttons));

#ifdef __linux__
  fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
  if (fd >= 0) {
    connected = true;
  }
#endif
}

Joystick::~Joystick() {
  if (fd >= 0) {
    close(fd);
  }
}

bool Joystick::isConnected() const { return connected; }

void Joystick::update() {
#ifdef __linux__
  if (!connected)
    return;

  struct js_event event;
  while (read(fd, &event, sizeof(event)) > 0) {
    if (event.type == JS_EVENT_AXIS) {
      if (event.number == 0)
        axisX = event.value;
      if (event.number == 1)
        axisY = event.value;
    } else if (event.type == JS_EVENT_BUTTON) {
      if (event.number < 16) {
        buttons[event.number] = (event.value != 0);
      }
    }
  }
#endif
}
