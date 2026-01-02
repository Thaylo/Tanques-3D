/**
 * Agent.cpp - Tank agent entity implementation
 *
 * Created: 15/10/2012
 * Author: thaylo
 *
 * Uses modern C++ features: range-based for, auto, algorithms
 * Renderer-agnostic: draw() stubbed for Vulkan implementation.
 */

#include "entities/Agent.h"
#include "entities/Projectile.h"
#include "game/GameData.h"

#include <algorithm>
#include <limits>

extern GameData *gameData;

Agent::Agent()
    : Controlable(), nearestEnemy(nullptr), id(0), markedForDestruction(false),
      reloadTimer(0), isFiring(false) {
  position = Vector(0, 0, 0);
  velocity = Vector(-2, 0, 0);
  dir = Vector(1, 0, 0);
  side = Vector(0, 1, 0);
  up = Vector(0, 0, 1);
}

Agent::Agent(Vector pos)
    : Controlable(), nearestEnemy(nullptr), id(0), markedForDestruction(false),
      reloadTimer(0), isFiring(false) {
  position = pos;
  velocity = Vector(-2, 0, 0);
  dir = Vector(1, 0, 0);
  side = Vector(0, 1, 0);
  up = Vector(0, 0, 1);
}

void Agent::setId(int idx) { id = idx; }
int Agent::getId() { return id; }

void Agent::draw() {
  // TODO: Vulkan rendering implementation
  // Tank geometry and textures will be rendered via VulkanRenderer
}

void Agent::drawRadar() {
  // TODO: Vulkan rendering implementation
  // Radar indicator for tracking nearest enemy
}

void Agent::iterate() {
  // Find nearest enemy using STL algorithm
  const auto &agentsVec = gameData->getAgentsVector();

  nearestEnemy = nullptr;

  auto nearestIt = std::min_element(
      agentsVec.begin(), agentsVec.end(), [this](const auto &a, const auto &b) {
        auto distA = (a->getId() != PLAYER_ID &&
                      dynamic_cast<Projectile *>(a.get()) == nullptr)
                         ? (a->getPosition() - position).getLengthVector()
                         : std::numeric_limits<double>::max();
        auto distB = (b->getId() != PLAYER_ID &&
                      dynamic_cast<Projectile *>(b.get()) == nullptr)
                         ? (b->getPosition() - position).getLengthVector()
                         : std::numeric_limits<double>::max();
        return distA < distB;
      });

  if (nearestIt != agentsVec.end()) {
    Agent *candidate = nearestIt->get();
    if (candidate->getId() != PLAYER_ID &&
        dynamic_cast<Projectile *>(candidate) == nullptr) {
      nearestEnemy = candidate;
    }
  }

  reloadTimer++;
  controlAction();
  Controlable::iterate();
}

void Agent::controlAction() {
  if (!control)
    return;

  double vert = 0, hor = 0;

  if (control->arrowUp)
    vert += 1;
  if (control->arrowDown)
    vert -= 1;
  if (control->arrowRight)
    hor += 1;
  if (control->arrowLeft)
    hor -= 1;

  Vector newDir = dir;

  if (control->space)
    shoot();

  // Tank always moves in direction it's facing
  acceleration = newDir.setVectorLength(MOVABLE_MAX_ACCELERATION) * vert;
  v_yaw = -hor;
}

void Agent::shoot() {
  if (reloadTimer > RELOAD_ROUNDS) {
    reloadTimer = 0;
    isFiring = true;
  }
}

void Agent::markForDestruction() { markedForDestruction = true; }

bool Agent::isMarkedForDestruction() { return markedForDestruction; }

bool Agent::checkFired() {
  if (isFiring) {
    isFiring = false;
    return true;
  }
  return false;
}

Agent::~Agent() {}
