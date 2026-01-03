/**
 * PhysicsWorld.mm - Physics world implementation
 *
 * Apple Silicon ONLY - no fallbacks.
 * Bridges Metal compute physics with game entities.
 */

#include "physics/PhysicsWorld.h"
#include <iostream>

#import <simd/simd.h>

namespace Physics {

PhysicsWorld::PhysicsWorld()
    : metalCompute_(std::make_unique<MetalCompute>()) {}

PhysicsWorld::~PhysicsWorld() { shutdown(); }

void PhysicsWorld::initialize() {
  metalCompute_->initialize(); // Throws on failure
  std::cout << "[PhysicsWorld] Initialized on "
            << metalCompute_->getDeviceName() << std::endl;
}

void PhysicsWorld::shutdown() {
  if (metalCompute_) {
    metalCompute_->shutdown();
  }
  bodies_.clear();
}

uint32_t PhysicsWorld::createBody(const Vector &position, float mass) {
  uint32_t index = static_cast<uint32_t>(bodies_.size());

  RigidBody body = {};
  body.position = simd_make_float3(static_cast<float>(position.getX()),
                                   static_cast<float>(position.getY()),
                                   static_cast<float>(position.getZ()));
  body.mass = mass;
  body.invMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
  body.velocity = simd_make_float3(0, 0, 0);
  body.angularVelocity = simd_make_float3(0, 0, 0);
  body.orientation = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f);
  body.force = simd_make_float3(0, 0, 0);
  body.torque = simd_make_float3(0, 0, 0);
  body.restitution = 0.3f; // Default bounciness
  bodies_.push_back(body);

  // Update Metal buffer
  metalCompute_->setRigidBodies(bodies_);

  return index;
}

void PhysicsWorld::setBodyPosition(uint32_t index, const Vector &pos) {
  if (index >= bodies_.size())
    return;

  bodies_[index].position = simd_make_float3(static_cast<float>(pos.getX()),
                                             static_cast<float>(pos.getY()),
                                             static_cast<float>(pos.getZ()));
}

void PhysicsWorld::setBodyVelocity(uint32_t index, const Vector &vel) {
  if (index >= bodies_.size())
    return;

  bodies_[index].velocity = simd_make_float3(static_cast<float>(vel.getX()),
                                             static_cast<float>(vel.getY()),
                                             static_cast<float>(vel.getZ()));
}

Vector PhysicsWorld::getBodyPosition(uint32_t index) const {
  if (index >= bodies_.size())
    return Vector(0, 0, 0);

  const auto &pos = bodies_[index].position;
  return Vector(pos.x, pos.y, pos.z);
}

Vector PhysicsWorld::getBodyVelocity(uint32_t index) const {
  if (index >= bodies_.size())
    return Vector(0, 0, 0);

  const auto &vel = bodies_[index].velocity;
  return Vector(vel.x, vel.y, vel.z);
}

void PhysicsWorld::applyForce(uint32_t index, const Vector &force) {
  if (index >= bodies_.size())
    return;

  bodies_[index].force += simd_make_float3(static_cast<float>(force.getX()),
                                           static_cast<float>(force.getY()),
                                           static_cast<float>(force.getZ()));
}

void PhysicsWorld::applyImpulse(uint32_t index, const Vector &impulse) {
  if (index >= bodies_.size())
    return;

  float invMass = bodies_[index].invMass;
  bodies_[index].velocity +=
      simd_make_float3(static_cast<float>(impulse.getX()) * invMass,
                       static_cast<float>(impulse.getY()) * invMass,
                       static_cast<float>(impulse.getZ()) * invMass);
}

void PhysicsWorld::step(float deltaTime) {
  if (bodies_.empty())
    return;

  metalCompute_->setRigidBodies(bodies_);
  metalCompute_->stepSimulation(deltaTime);
  metalCompute_->getRigidBodies(bodies_);
}

void PhysicsWorld::setGravity(float g) { metalCompute_->setGravity(g); }

std::string PhysicsWorld::getDeviceName() const {
  return metalCompute_->getDeviceName();
}

uint32_t PhysicsWorld::getBodyCount() const {
  return static_cast<uint32_t>(bodies_.size());
}

} // namespace Physics
