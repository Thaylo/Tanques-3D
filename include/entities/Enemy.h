/**
 * Enemy.h - AI-controlled enemy tank
 *
 * Created: 19/04/2013
 * Author: thaylo
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "entities/Agent.h"

/**
 * Enemy tank that automatically targets and attacks the player.
 */
class Enemy : public Agent {
private:
  Agent *target;

public:
  Enemy();
  explicit Enemy(Agent *targetAgent);
  virtual ~Enemy();

  void controlAction() override;
  void shoot() override;
};

#endif // ENEMY_H
