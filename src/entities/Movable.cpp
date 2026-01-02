/**
 * Movable.cpp - Base class for moving entities implementation
 *
 * Created: 14/10/2012
 * Author: thaylo
 *
 * C++23 modernization
 */

#include "entities/Movable.h"

#include <algorithm> // std::clamp (C++17+)

void Movable::iterate() {
  // Update angular state
  constexpr double timeScale = TIME_STEP / 500.0;
  roll += v_roll * timeScale;
  pitch += v_pitch * timeScale;
  yaw += v_yaw * timeScale;

  // Update position using CURRENT velocity (before clamping)
  // This allows projectiles to set high velocity that gets applied
  constexpr double deltaTime = TIME_STEP / 1000.0;
  position = position + velocity * deltaTime;

  // Apply acceleration and friction
  velocity = velocity + acceleration * deltaTime -
             velocity * MOVABLE_LINEAR_FRICTION * deltaTime;

  // Clamp velocity AFTER position update (for tanks, not projectiles)
  // Projectiles override velocity each frame anyway
  if (velocity.getLengthSquared() >
      MOVABLE_MAX_VELOCITY * MOVABLE_MAX_VELOCITY) {
    velocity.setVectorLength(MOVABLE_MAX_VELOCITY);
  }

  // Update orientation vectors
  up = Vector(0, 0, 1);
  side = side.rotateVector(up, v_yaw * deltaTime).setVectorLength(1.0);
  dir = up.crossProduct(side).setVectorLength(1.0);
}
