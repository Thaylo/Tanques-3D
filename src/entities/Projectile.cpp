/**
 * Projectile.cpp - Projectile entity implementation (formerly Projetil)
 *
 * Created: 18/04/2013
 * Author: thaylo
 *
 * Uses modern C++ features: range-based for, auto, algorithms
 */

#include "entities/Projectile.h"
#include "game/GameData.h"

#include <algorithm>

extern GameData *gameData;

Projectile::Projectile() : Agent(), distance(0) {
  initialPos = position;
  position = Vector(0, 0, 0);
  // Prevents camera initialization issue (workaround)
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
  glColor3f(1, 0, 0);
  glTranslated(position.getX(), position.getY(), position.getZ());

  constexpr double size = 1 / 5.0;

  // Draw projectile as a set of triangles forming a pointed shape
  glBegin(GL_TRIANGLES);
  glVectorT(dir * 2 * size);
  glVectorT(side * (-size / 2) - up * (size / 5));
  glVectorT(side * (size / 2) - up * (size / 5));
  glEnd();

  glBegin(GL_TRIANGLES);
  glVectorT(dir * 2 * size);
  glVectorT(side * (-size / 2) + up * (size / 5));
  glVectorT(side * (size / 2) + up * (size / 5));
  glEnd();

  glBegin(GL_TRIANGLES);
  glVectorT(dir * 2 * size);
  glVectorT(side * (-size / 2) + up * (size / 5));
  glVectorT(side * (-size / 2) - up * (size / 5));
  glEnd();

  glBegin(GL_TRIANGLES);
  glVectorT(dir * 2 * size);
  glVectorT(side * (size / 2) + up * (size / 5));
  glVectorT(side * (size / 2) - up * (size / 5));
  glEnd();

  glTranslated(-position.getX(), -position.getY(), -position.getZ());
}

void Projectile::iterate() {
  Agent::iterate();
  velocity.setVectorLength(MOVABLE_MAX_VELOCITY);

  const auto &agentsVec = gameData->getAgentsVector();
  constexpr double collisionRadius = 0.5;
  constexpr double maxTravelDistance = 40.0;

  // Check collision with any agent using STL algorithm
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
