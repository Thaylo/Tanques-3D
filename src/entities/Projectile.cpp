/**
 * Projectile.cpp - Projectile entity implementation
 *
 * Created: 18/04/2013
 * Author: thaylo
 *
 * Uses modern C++ features: range-based for, auto, algorithms
 * Renderer-agnostic: draw() stubbed for Vulkan implementation.
 */

#include "entities/Projectile.h"
#include "game/GameData.h"

#include <algorithm>

extern GameData *gameData;

Projectile::Projectile() : Agent(), distance(0) {
  initialPos = position;
  position = Vector(0, 0, 0);
  velocity = Vector(0.0001, 0, 0);
  dir = Vector(1, 0, 0);
  side = Vector(0, 1, 0);
  up = Vector(0, 0, 1);
  isFiring = false;
}

Projectile::Projectile(Agent *shooter) : Agent(), distance(0) {
  initialPos = shooter->getPosition();
  position = shooter->getPosition();
  velocity = shooter->getDir();
  velocity.setVectorLength(MOVABLE_MAX_VELOCITY * 3);

  dir = shooter->getDir();
  side = shooter->getSide();
  up = shooter->getUp();
  isFiring = false;
}

void Projectile::draw() {
  // TODO: Vulkan rendering implementation
  // Projectile rendered as pointed triangular shape
}

void Projectile::iterate() {
  Agent::iterate();
  velocity.setVectorLength(MOVABLE_MAX_VELOCITY);

  const auto &agentsVec = gameData->getAgentsVector();
  constexpr double collisionRadius = 0.5;
  constexpr double maxTravelDistance = 40.0;

  // Check collision with any agent
  std::for_each(agentsVec.begin(), agentsVec.end(), [this](const auto &agent) {
    double dist = (agent->getPosition() - position).getLengthVector();

    // Check collision (but not with self)
    if (dist < collisionRadius && this->getId() != agent->getId()) {
      agent->markForDestruction();
    }
  });

  // Destroy after traveling max distance
  if ((position - initialPos).getLengthVector() > maxTravelDistance) {
    markForDestruction();
  }
}

Projectile::~Projectile() {}
