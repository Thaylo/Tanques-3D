/**
 * Control.h - Input control state
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "core/Vector.h"

/**
 * Structure holding the current state of all input controls.
 */
struct Control {
  // Arrow key states
  bool arrowUp;
  bool arrowDown;
  bool arrowLeft;
  bool arrowRight;

  // Turbine power controls
  bool power_inc;
  bool power_dec;

  // Mouse button states
  bool rightPressed;
  bool newRightPressed;
  bool leftPressed;
  bool newLeftPressed;
  bool scrollPressed;

  // Action keys
  bool space;
  bool keyEsc;

  // Mouse positions
  Vector pressedPos;
  Vector releasedPos;
};

/**
 * Creates and initializes a Control structure with all values set to
 * false/zero.
 */
Control initializeControl();

#endif // CONTROL_H
