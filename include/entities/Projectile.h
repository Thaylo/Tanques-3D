/**
 * Projectile.h - Projectile entity (formerly Projetil)
 *
 * Created: 18/04/2013
 * Author: thaylo
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entities/Agent.h"

/**
 * Projectile fired by an Agent. Inherits from Agent for shared behavior.
 */
class Projectile : public Agent {
private:
  Vector initialPos;
  double distance;

public:
  Projectile();
  explicit Projectile(Agent *shooter);
  virtual ~Projectile();

  void draw() override;
  void iterate() override;
};

#endif // PROJECTILE_H
