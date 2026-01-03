/**
 * test_metal_physics.mm - Metal physics integration tests
 *
 * Tests Metal compute physics on Apple Silicon.
 */

#include "physics/PhysicsWorld.h"
#include <gtest/gtest.h>
#include <iostream>

namespace {

TEST(MetalPhysics, InitializationTest) {
  Physics::PhysicsWorld world;

  // Should not throw on Apple Silicon
  EXPECT_NO_THROW(world.initialize());

  std::cout << "GPU: " << world.getDeviceName() << std::endl;
  EXPECT_TRUE(world.getDeviceName().find("Apple") != std::string::npos);

  world.shutdown();
}

TEST(MetalPhysics, RigidBodyCreation) {
  Physics::PhysicsWorld world;
  world.initialize();

  // Create a rigid body
  uint32_t bodyId = world.createBody(Vector(10, 20, 5), 100.0f);
  EXPECT_EQ(bodyId, 0);
  EXPECT_EQ(world.getBodyCount(), 1);

  // Verify position
  Vector pos = world.getBodyPosition(bodyId);
  EXPECT_NEAR(pos.getX(), 10.0, 0.001);
  EXPECT_NEAR(pos.getY(), 20.0, 0.001);
  EXPECT_NEAR(pos.getZ(), 5.0, 0.001);

  world.shutdown();
}

TEST(MetalPhysics, PhysicsSimulation) {
  Physics::PhysicsWorld world;
  world.initialize();
  world.setGravity(10.0f); // 10 m/s² for easy math

  // Create a body at height Z=100
  uint32_t bodyId = world.createBody(Vector(0, 0, 100), 1.0f);

  // Initial position
  Vector pos0 = world.getBodyPosition(bodyId);
  EXPECT_NEAR(pos0.getZ(), 100.0, 0.001);

  // Simulate 1 second (50 steps of 0.02s)
  for (int i = 0; i < 50; i++) {
    world.step(0.02f);
  }

  // After 1 second of 10 m/s² gravity, should have fallen ~5m
  // (d = 0.5 * g * t² = 0.5 * 10 * 1 = 5m)
  Vector pos1 = world.getBodyPosition(bodyId);
  std::cout << "After 1s: Z = " << pos1.getZ() << " (expected ~95)"
            << std::endl;

  EXPECT_LT(pos1.getZ(), pos0.getZ()); // Should have fallen
  EXPECT_NEAR(pos1.getZ(), 95.0, 2.0); // ~5m drop with some tolerance

  world.shutdown();
}

} // namespace
