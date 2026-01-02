/**
 * Controlable.cpp - Base class for controllable entities implementation
 */

#include "entities/Controlable.h"

Controlable::Controlable() : Movable(), control(nullptr) {}

void Controlable::setController(Control *ctrl) { control = ctrl; }
