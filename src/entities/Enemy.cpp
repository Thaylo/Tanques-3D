/**
 * Enemy.cpp - AI-controlled enemy tank implementation
 *
 * Created: 19/04/2013
 * Author: thaylo
 */

#include "entities/Enemy.h"

Enemy::Enemy() : Agent(), target(nullptr) { control = nullptr; }

Enemy::Enemy(Agent *targetAgent) : Agent(), target(targetAgent) {
  control = nullptr;
}

void Enemy::controlAction() {
  if (target == nullptr) {
    std::cout << "Target not initialized for enemy\n";
    return;
  }

  Vector toTarget = target->getPosition() - position;
  double sine = toTarget.crossProduct(dir).getLengthVector();
  double cosine = toTarget.dotProduct(dir);
  double theta = atan2(sine, cosine);

  // Turn towards target
  if (theta > 0.02 && theta < M_PI) {
    v_yaw = 1;
  } else if (theta < -0.02 && theta >= -M_PI) {
    v_yaw = -1;
  } else {
    v_yaw = 0;
  }

  Vector newDir = dir;
  double distanceToTarget = toTarget.getLengthVector();

  // Move towards target if far away
  if (distanceToTarget > 7) {
    acceleration = newDir.setVectorLength(MOVABLE_MAX_ACCELERATION);
  } else {
    acceleration = 0;
    // Shoot if close and aimed
    if (distanceToTarget < 13 && fabs(theta) < M_PI / 15) {
      shoot();
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
