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
  velocity.setVectorLength(PROJECTILE_SPEED); // Use projectile-specific speed

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
  // Set velocity BEFORE parent iterate() to bypass MOVABLE_MAX_VELOCITY
  // clamping
  velocity.setVectorLength(PROJECTILE_SPEED);

  // Skip Agent behavior (no control, no enemy tracking)
  // Just do basic Movable physics
  Movable::iterate();

  const auto &agentsVec = gameData->getAgentsVector();
  constexpr double collisionRadius = 4.0; // 4 meters (tank is ~8m long)
  const double maxTravelDistance = PROJECTILE_MAX_DISTANCE;

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
