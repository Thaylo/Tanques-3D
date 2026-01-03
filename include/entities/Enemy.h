/**
 * Enemy.h - AI-controlled enemy tank
 *
 * Uses trained neural network via AIController for decisions.
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "entities/Agent.h"

// Forward declaration
namespace AI {
class AIController;
}

/**
 * Enemy tank that uses trained neural network AI.
 */
class Enemy : public Agent {
private:
  Agent *target;
  AI::AIController *aiController_ = nullptr;

public:
  Enemy();
  explicit Enemy(Agent *targetAgent);
  virtual ~Enemy();

  void setAIController(AI::AIController *controller) {
    aiController_ = controller;
  }

  void controlAction() override;
  void shoot() override;
};

#endif // ENEMY_H
