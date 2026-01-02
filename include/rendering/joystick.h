/**
 * joystick.h - Joystick input handling
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

/**
 * Joystick input handler (Linux only).
 */
class Joystick {
private:
  int fd;
  bool connected;

public:
  Joystick();
  ~Joystick();

  bool isConnected() const;
  void update();

  // Axis values (-32767 to 32767)
  int axisX;
  int axisY;

  // Button states
  bool buttons[16];
};

#endif // JOYSTICK_H
