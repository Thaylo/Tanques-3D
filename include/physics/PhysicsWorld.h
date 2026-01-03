/**
 * PhysicsWorld.h - High-level physics world manager
 *
 * Apple Silicon ONLY - no fallbacks.
 * Bridges Metal compute physics with game entities.
 */

#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <memory>
#include <vector>

#include "core/Vector.h"
#include "physics/MetalCompute.h"

namespace Physics {

/**
 * PhysicsWorld - Game-facing physics interface
 *
 * Requires Apple Silicon GPU - throws on initialization failure.
 */
class PhysicsWorld {
public:
  PhysicsWorld();
  ~PhysicsWorld();

  // Initialize physics system - throws on failure
  void initialize();

  // Shutdown physics
  void shutdown();

  // Create a rigid body, returns index
  uint32_t createBody(const Vector &position, float mass);

  // Update body state from game entity
  void setBodyPosition(uint32_t index, const Vector &pos);
  void setBodyVelocity(uint32_t index, const Vector &vel);

  // Get body state for game entity
  Vector getBodyPosition(uint32_t index) const;
  Vector getBodyVelocity(uint32_t index) const;

  // Apply forces
  void applyForce(uint32_t index, const Vector &force);
  void applyImpulse(uint32_t index, const Vector &impulse);

  // Simulation
  void step(float deltaTime);

  // Configuration
  void setGravity(float g);

  // Debug
  std::string getDeviceName() const;
  uint32_t getBodyCount() const;

private:
  std::unique_ptr<MetalCompute> metalCompute_;
  std::vector<RigidBody> bodies_;
};

} // namespace Physics

#endif // PHYSICS_WORLD_H
