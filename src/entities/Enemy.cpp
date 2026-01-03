/**
 * Enemy.cpp - AI-controlled enemy tank using trained neural network
 */

#include "entities/Enemy.h"
#include "ai/AIController.h"
#include <cmath>

Enemy::Enemy() : Agent(), target(nullptr) { control = nullptr; }

Enemy::Enemy(Agent *targetAgent) : Agent(), target(targetAgent) {
  control = nullptr;
}

void Enemy::controlAction() {
  if (target == nullptr) {
    return;
  }

  Vector toTarget = target->getPosition() - position;
  double distanceToTarget = toTarget.getLengthVector();

  // Calculate angle to target
  double sine = toTarget.crossProduct(dir).getLengthVector();
  double cosine = toTarget.dotProduct(dir);
  double theta = atan2(sine, cosine);

  // Use trained neural network if available
  if (aiController_) {
    AI::AIInput input = {
        .distanceToTarget = static_cast<float>(distanceToTarget),
        .angleToTarget = static_cast<float>(theta),
        .targetVelocityX = static_cast<float>(target->getVelocity().getX()),
        .targetVelocityY = static_cast<float>(target->getVelocity().getY()),
        .myVelocityX = static_cast<float>(velocity.getX()),
        .myVelocityY = static_cast<float>(velocity.getY()),
        .myHealth = 100.0f, // TODO: Add health tracking
        .targetHealth = 100.0f};

    AI::AIDecision decision = aiController_->predict(input);

    // Apply neural network decisions

    // Turn: decision.turnDirection is -1 to 1
    if (decision.turnDirection > 0.1f) {
      v_yaw = 1;
    } else if (decision.turnDirection < -0.1f) {
      v_yaw = -1;
    } else {
      v_yaw = 0;
    }

    // Throttle: decision.throttle is 0 to 1
    if (decision.throttle > 0.3f) {
      acceleration =
          dir.setVectorLength(MOVABLE_MAX_ACCELERATION * decision.throttle);
    } else {
      acceleration = Vector(0, 0, 0);
    }

    // Shoot: decision.shouldShoot is 0 to 1
    if (decision.shouldShoot > 0.5f) {
      shoot();
    }

  } else {
    // Fallback: old heuristic behavior
    if (theta > 0.02 && theta < M_PI) {
      v_yaw = 1;
    } else if (theta < -0.02 && theta >= -M_PI) {
      v_yaw = -1;
    } else {
      v_yaw = 0;
    }

    if (distanceToTarget > 7) {
      acceleration = dir.setVectorLength(MOVABLE_MAX_ACCELERATION);
    } else {
      acceleration = Vector(0, 0, 0);
      if (distanceToTarget < 13 && fabs(theta) < M_PI / 15) {
        shoot();
      }
    }
  }
}

void Enemy::shoot() {
  // AI has a handicap - slower reload
  if (reloadTimer > RELOAD_ROUNDS * RELOAD_HANDICAP_FOR_AI) {
    Agent::shoot();
  }
}

Enemy::~Enemy() {}
