/**
 * Controlable.h - Base class for controllable entities
 */

#ifndef CONTROLABLE_H
#define CONTROLABLE_H

#include "entities/Movable.h"
#include "game/Control.h"

/**
 * Abstract base class for entities that can be controlled by a Control input.
 */
class Controlable : public Movable {
protected:
  Control *control;

public:
  Controlable();
  virtual ~Controlable() {}

  void setController(Control *ctrl);

  virtual void controlAction() = 0;
};

#endif // CONTROLABLE_H
