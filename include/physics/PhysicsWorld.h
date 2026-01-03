/**
 * PhysicsWorld.h - High-level physics world manager
 *
 * Bridges Metal compute physics with game entities.
 * Manages rigid bodies, forces, and simulation stepping.
 */

#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <memory>
#include <vector>

#ifdef __APPLE__
#include "physics/MetalCompute.h"
#endif

#include "core/Vector.h"

namespace Physics {

/**
 * PhysicsWorld - Game-facing physics interface
 *
 * Abstracts Metal compute details and provides simple API
 * for game entities to interact with physics simulation.
 */
class PhysicsWorld {
public:
  PhysicsWorld();
  ~PhysicsWorld();

  // Initialize physics system
  bool initialize();

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
  bool isGPUAccelerated() const;
  uint32_t getBodyCount() const;

private:
#ifdef __APPLE__
  std::unique_ptr<MetalCompute> metalCompute_;
  std::vector<RigidBody> bodies_;
#else
  // CPU-only fallback for non-Apple platforms
  struct SimplifiedBody {
    Vector position;
    Vector velocity;
    float mass;
  };
  std::vector<SimplifiedBody> bodies_;
  float gravity_ = 9.81f;
#endif
};

} // namespace Physics

#endif // PHYSICS_WORLD_H
