/**
 * Control.cpp - Input control implementation
 */

#include "game/Control.h"

Control initializeControl() {
  Control control;

  control.power_dec = control.power_inc = false;
  control.arrowUp = false;
  control.arrowDown = false;
  control.arrowLeft = false;
  control.arrowRight = false;

  control.newRightPressed = false;
  control.rightPressed = false;
  control.newLeftPressed = false;
  control.leftPressed = false;
  control.scrollPressed = false;

  control.keyEsc = false;
  control.space = false;

  control.pressedPos = Vector(0, 0, 0);
  control.releasedPos = Vector(0, 0, 0);

  return control;
}
