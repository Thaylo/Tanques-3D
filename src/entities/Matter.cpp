/**
 * Matter.cpp - Physical matter properties implementation
 */

#include "entities/Matter.h"

Matter::Matter() : mass(1.0) {}

double Matter::getMass() const { return mass; }
void Matter::setMass(double m) { mass = m; }
