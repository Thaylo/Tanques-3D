/**
 * test_projectile_physics.cpp - Unit tests for projectile speed and physics
 *
 * These tests verify that projectiles travel at the correct SI speed.
 */

#include "core/Constants.h"
#include "core/Vector.h"
#include "entities/Movable.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

// Test that physics constants are as expected
TEST(ProjectilePhysics, ConstantsAreSI) {
  std::cout << "\n=== Physics Constants (SI Units) ===" << std::endl;
  std::cout << "TIME_STEP: " << TIME_STEP << " ms" << std::endl;
  std::cout << "MOVABLE_MAX_VELOCITY: " << MOVABLE_MAX_VELOCITY << " m/s"
            << std::endl;
  std::cout << "MOVABLE_MAX_ACCELERATION: " << MOVABLE_MAX_ACCELERATION
            << " m/s²" << std::endl;
  std::cout << "PROJECTILE_SPEED: " << PROJECTILE_SPEED << " m/s" << std::endl;
  std::cout << "PROJECTILE_MAX_DISTANCE: " << PROJECTILE_MAX_DISTANCE << " m"
            << std::endl;

  // Verify expected values
  EXPECT_EQ(TIME_STEP, 20);
  EXPECT_DOUBLE_EQ(PROJECTILE_SPEED, 200.0);
  EXPECT_DOUBLE_EQ(MOVABLE_MAX_VELOCITY, 25.0); // 25 m/s = 90 km/h (arcade)
}

// Test deltaTime calculation
TEST(ProjectilePhysics, DeltaTimeCalculation) {
  constexpr double deltaTime = TIME_STEP / 1000.0;
  std::cout << "\n=== Delta Time ===" << std::endl;
  std::cout << "deltaTime: " << deltaTime << " seconds" << std::endl;

  EXPECT_DOUBLE_EQ(deltaTime, 0.02); // 20ms = 0.02s
}

// Test expected projectile movement per tick
TEST(ProjectilePhysics, ProjectileMovementPerTick) {
  constexpr double deltaTime = TIME_STEP / 1000.0;
  double expectedMovement = PROJECTILE_SPEED * deltaTime;

  std::cout << "\n=== Expected Projectile Movement ===" << std::endl;
  std::cout << "PROJECTILE_SPEED: " << PROJECTILE_SPEED << " m/s" << std::endl;
  std::cout << "deltaTime: " << deltaTime << " s" << std::endl;
  std::cout << "Expected movement per tick: " << expectedMovement << " m"
            << std::endl;
  std::cout << "Expected movement per second (50 ticks): "
            << expectedMovement * 50 << " m/s" << std::endl;

  // At 200 m/s and 0.02s per tick, should move 4 meters per tick
  EXPECT_DOUBLE_EQ(expectedMovement, 4.0);
}

// Simulate projectile movement WITHOUT velocity clamping
TEST(ProjectilePhysics, SimulateProjectileMovement) {
  std::cout << "\n=== Simulating Projectile Movement ===" << std::endl;

  Movable projectile;
  projectile.setPosition(Vector(0, 0, 0));
  projectile.setVelocity(
      Vector(PROJECTILE_SPEED, 0, 0)); // 200 m/s in X direction

  Vector initialPos = projectile.getPosition();
  std::cout << "Initial position: (" << initialPos.getX() << ", "
            << initialPos.getY() << ")" << std::endl;
  std::cout << "Initial velocity: "
            << projectile.getVelocity().getLengthVector() << " m/s"
            << std::endl;

  // Run one physics tick
  projectile.iterate();

  Vector afterOneTickPos = projectile.getPosition();
  double distanceMoved = (afterOneTickPos - initialPos).getLengthVector();

  std::cout << "After 1 tick position: (" << afterOneTickPos.getX() << ", "
            << afterOneTickPos.getY() << ")" << std::endl;
  std::cout << "Velocity after iterate(): "
            << projectile.getVelocity().getLengthVector() << " m/s"
            << std::endl;
  std::cout << "Distance moved in 1 tick: " << distanceMoved << " m"
            << std::endl;

  // KEY INSIGHT: If velocity is clamped to 15 m/s, movement will be 15 * 0.02 =
  // 0.3m If velocity is NOT clamped, movement will be 200 * 0.02 = 4m

  // Check if velocity was clamped
  if (projectile.getVelocity().getLengthVector() < PROJECTILE_SPEED - 1) {
    std::cout << "WARNING: Velocity was clamped from " << PROJECTILE_SPEED
              << " to " << projectile.getVelocity().getLengthVector() << " m/s!"
              << std::endl;
    std::cout
        << "This is the bug! Movable::iterate() clamps to MOVABLE_MAX_VELOCITY."
        << std::endl;
  }

  // Now that clamping happens AFTER position update, we move the full distance
  EXPECT_NEAR(distanceMoved, 4.0, 0.1); // Should be ~4m at 200 m/s
}

// Test what projectile movement SHOULD be without clamping
TEST(ProjectilePhysics, ProjectileMovementWithoutClamping) {
  std::cout << "\n=== Projectile Movement (Bypassing Clamp) ===" << std::endl;

  // Manually simulate what should happen:
  constexpr double deltaTime = TIME_STEP / 1000.0;

  Vector position(0, 0, 0);
  Vector velocity(PROJECTILE_SPEED, 0, 0); // 200 m/s

  // Update position (no clamping)
  position = position + velocity * deltaTime;

  std::cout << "Velocity: " << velocity.getLengthVector() << " m/s"
            << std::endl;
  std::cout << "Position after 1 tick: " << position.getX() << " m"
            << std::endl;

  // Should move 4 meters
  EXPECT_DOUBLE_EQ(position.getX(), 4.0);

  // After 1 second (50 ticks), should move 200 meters
  Vector pos1sec(0, 0, 0);
  for (int i = 0; i < 50; i++) {
    pos1sec = pos1sec + velocity * deltaTime;
  }
  std::cout << "Position after 50 ticks (1 second): " << pos1sec.getX() << " m"
            << std::endl;
  EXPECT_NEAR(pos1sec.getX(), 200.0, 0.001);
}

// Test tank acceleration
TEST(ProjectilePhysics, TankAccelerationTest) {
  std::cout << "\n=== Tank Acceleration Test ===" << std::endl;

  Movable tank;
  tank.setPosition(Vector(0, 0, 0));
  tank.setVelocity(Vector(0, 0, 0));
  tank.setAcceleration(Vector(MOVABLE_MAX_ACCELERATION, 0, 0)); // 10 m/s²

  std::cout << "Acceleration: " << MOVABLE_MAX_ACCELERATION << " m/s²"
            << std::endl;
  std::cout << "Friction: " << MOVABLE_LINEAR_FRICTION << std::endl;
  std::cout << "Max velocity: " << MOVABLE_MAX_VELOCITY << " m/s" << std::endl;

  // Simulate 1 second (50 ticks)
  for (int i = 0; i < 50; i++) {
    tank.iterate();
    if (i == 0 || i == 9 || i == 24 || i == 49) {
      std::cout << "Tick " << (i + 1)
                << ": vel=" << tank.getVelocity().getLengthVector()
                << " m/s, pos=" << tank.getPosition().getX() << " m"
                << std::endl;
    }
  }

  double finalVel = tank.getVelocity().getLengthVector();
  double finalPos = tank.getPosition().getX();

  std::cout << "After 1 second: velocity=" << finalVel
            << " m/s, position=" << finalPos << " m" << std::endl;

  // With accel=10, friction=0.3, equilibrium is vel = accel/friction = 33 m/s
  // But capped at 20 m/s, so should reach 20 m/s
  EXPECT_GE(finalVel, 15.0); // Should reach at least 15 m/s after 1 second
}
