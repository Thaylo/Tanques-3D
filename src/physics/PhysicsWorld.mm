/**
 * PhysicsWorld.mm - Physics world implementation
 *
 * Bridges Metal compute physics with game entities.
 */

#include "physics/PhysicsWorld.h"
#include <iostream>

#ifdef __APPLE__
#import <simd/simd.h>
#endif

namespace Physics {

PhysicsWorld::PhysicsWorld() {
#ifdef __APPLE__
  metalCompute_ = std::make_unique<MetalCompute>();
#endif
}

PhysicsWorld::~PhysicsWorld() { shutdown(); }

bool PhysicsWorld::initialize() {
#ifdef __APPLE__
  if (!metalCompute_->initialize()) {
    std::cerr << "[PhysicsWorld] Failed to initialize Metal compute"
              << std::endl;
    return false;
  }
  std::cout << "[PhysicsWorld] Initialized with "
            << (metalCompute_->isAvailable() ? "GPU acceleration"
                                             : "CPU fallback")
            << std::endl;
  return true;
#else
  std::cout << "[PhysicsWorld] Initialized (CPU only)" << std::endl;
  return true;
#endif
}

void PhysicsWorld::shutdown() {
#ifdef __APPLE__
  if (metalCompute_) {
    metalCompute_->shutdown();
  }
#endif
  bodies_.clear();
}

uint32_t PhysicsWorld::createBody(const Vector &position, float mass) {
  uint32_t index = static_cast<uint32_t>(bodies_.size());

#ifdef __APPLE__
  RigidBody body = {};
  body.position =
      simd_make_float3(position.getX(), position.getY(), position.getZ());
  body.mass = mass;
  body.invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
  body.velocity = simd_make_float3(0, 0, 0);
  body.angularVelocity = simd_make_float3(0, 0, 0);
  body.orientation = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f); // Identity
  body.force = simd_make_float3(0, 0, 0);
  body.torque = simd_make_float3(0, 0, 0);
  bodies_.push_back(body);

  // Update Metal buffer
  metalCompute_->setRigidBodies(bodies_);
#else
  SimplifiedBody body;
  body.position = position;
  body.velocity = Vector(0, 0, 0);
  body.mass = mass;
  bodies_.push_back(body);
#endif

  return index;
}

void PhysicsWorld::setBodyPosition(uint32_t index, const Vector &pos) {
  if (index >= bodies_.size())
    return;

#ifdef __APPLE__
  bodies_[index].position =
      simd_make_float3(pos.getX(), pos.getY(), pos.getZ());
#else
  bodies_[index].position = pos;
#endif
}

void PhysicsWorld::setBodyVelocity(uint32_t index, const Vector &vel) {
  if (index >= bodies_.size())
    return;

#ifdef __APPLE__
  bodies_[index].velocity =
      simd_make_float3(vel.getX(), vel.getY(), vel.getZ());
#else
  bodies_[index].velocity = vel;
#endif
}

Vector PhysicsWorld::getBodyPosition(uint32_t index) const {
  if (index >= bodies_.size())
    return Vector(0, 0, 0);

#ifdef __APPLE__
  const auto &pos = bodies_[index].position;
  return Vector(pos.x, pos.y, pos.z);
#else
  return bodies_[index].position;
#endif
}

Vector PhysicsWorld::getBodyVelocity(uint32_t index) const {
  if (index >= bodies_.size())
    return Vector(0, 0, 0);

#ifdef __APPLE__
  const auto &vel = bodies_[index].velocity;
  return Vector(vel.x, vel.y, vel.z);
#else
  return bodies_[index].velocity;
#endif
}

void PhysicsWorld::applyForce(uint32_t index, const Vector &force) {
  if (index >= bodies_.size())
    return;

#ifdef __APPLE__
  bodies_[index].force +=
      simd_make_float3(force.getX(), force.getY(), force.getZ());
#else
  // CPU fallback: apply acceleration directly
  if (bodies_[index].mass > 0) {
    bodies_[index].velocity =
        bodies_[index].velocity +
        force * (1.0f / bodies_[index].mass) * 0.02f; // Approximate
  }
#endif
}

void PhysicsWorld::applyImpulse(uint32_t index, const Vector &impulse) {
  if (index >= bodies_.size())
    return;

#ifdef __APPLE__
  float invMass = bodies_[index].invMass;
  bodies_[index].velocity +=
      simd_make_float3(impulse.getX() * invMass, impulse.getY() * invMass,
                       impulse.getZ() * invMass);
#else
  if (bodies_[index].mass > 0) {
    bodies_[index].velocity =
        bodies_[index].velocity + impulse * (1.0f / bodies_[index].mass);
  }
#endif
}

void PhysicsWorld::step(float deltaTime) {
#ifdef __APPLE__
  metalCompute_->setRigidBodies(bodies_);
  metalCompute_->stepSimulation(deltaTime);
  metalCompute_->getRigidBodies(bodies_);
#else
  // CPU fallback simulation
  for (auto &body : bodies_) {
    if (body.mass > 0) {
      // Apply gravity
      body.velocity.setZ(body.velocity.getZ() - gravity_ * deltaTime);
      // Integrate
      body.position = body.position + body.velocity * deltaTime;
    }
  }
#endif
}

void PhysicsWorld::setGravity(float g) {
#ifdef __APPLE__
  metalCompute_->setGravity(g);
#else
  gravity_ = g;
#endif
}

bool PhysicsWorld::isGPUAccelerated() const {
#ifdef __APPLE__
  return metalCompute_ && metalCompute_->isAvailable();
#else
  return false;
#endif
}

uint32_t PhysicsWorld::getBodyCount() const {
  return static_cast<uint32_t>(bodies_.size());
}

} // namespace Physics
