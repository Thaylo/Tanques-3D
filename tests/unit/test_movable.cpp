/**
 * test_movable.cpp - Unit tests for Movable class
 */

#include "entities/Movable.h"
#include <gtest/gtest.h>

// Constructor tests
TEST(MovableTest, DefaultConstructor) {
  Movable m;
  EXPECT_DOUBLE_EQ(m.getPosition().getX(), 0.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getY(), 0.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getZ(), 0.0);
}

TEST(MovableTest, PositionConstructor) {
  Movable m(Vector(5, 10, 15));
  EXPECT_DOUBLE_EQ(m.getPosition().getX(), 5.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getY(), 10.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getZ(), 15.0);
}

// Getter tests
TEST(MovableTest, DefaultVelocityIsZero) {
  Movable m;
  EXPECT_DOUBLE_EQ(m.getVelocity().getX(), 0.0);
  EXPECT_DOUBLE_EQ(m.getVelocity().getY(), 0.0);
  EXPECT_DOUBLE_EQ(m.getVelocity().getZ(), 0.0);
}

TEST(MovableTest, DefaultOrientationVectors) {
  Movable m;
  // Dir should be (1, 0, 0)
  EXPECT_DOUBLE_EQ(m.getDir().getX(), 1.0);
  EXPECT_DOUBLE_EQ(m.getDir().getY(), 0.0);

  // Up should be (0, 0, 1)
  EXPECT_DOUBLE_EQ(m.getUp().getZ(), 1.0);

  // Side should be (0, 1, 0)
  EXPECT_DOUBLE_EQ(m.getSide().getY(), 1.0);
}

// Setter tests
TEST(MovableTest, SetPosition) {
  Movable m;
  m.setPosition(Vector(7, 8, 9));
  EXPECT_DOUBLE_EQ(m.getPosition().getX(), 7.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getY(), 8.0);
  EXPECT_DOUBLE_EQ(m.getPosition().getZ(), 9.0);
}

TEST(MovableTest, SetVelocity) {
  Movable m;
  m.setVelocity(Vector(1, 2, 3));
  EXPECT_DOUBLE_EQ(m.getVelocity().getX(), 1.0);
  EXPECT_DOUBLE_EQ(m.getVelocity().getY(), 2.0);
  EXPECT_DOUBLE_EQ(m.getVelocity().getZ(), 3.0);
}

TEST(MovableTest, SetAcceleration) {
  Movable m;
  m.setAcceleration(Vector(0.5, 0.5, 0.5));
  EXPECT_DOUBLE_EQ(m.getAcceleration().getX(), 0.5);
}

// Euler angle tests
TEST(MovableTest, DefaultEulerAnglesAreZero) {
  Movable m;
  EXPECT_DOUBLE_EQ(m.getRoll(), 0.0);
  EXPECT_DOUBLE_EQ(m.getPitch(), 0.0);
  EXPECT_DOUBLE_EQ(m.getYaw(), 0.0);
}

TEST(MovableTest, SetEulerAngles) {
  Movable m;
  m.setRoll(1.0);
  m.setPitch(2.0);
  m.setYaw(3.0);
  EXPECT_DOUBLE_EQ(m.getRoll(), 1.0);
  EXPECT_DOUBLE_EQ(m.getPitch(), 2.0);
  EXPECT_DOUBLE_EQ(m.getYaw(), 3.0);
}

TEST(MovableTest, SetAngularVelocities) {
  Movable m;
  m.setVRoll(0.1);
  m.setVPitch(0.2);
  m.setVYaw(0.3);
  // No getters for v_roll etc, but we can verify iterate() works
  m.iterate();
  // After one iteration, roll should be non-zero
  EXPECT_NE(m.getRoll(), 0.0);
}

// Physics simulation tests
TEST(MovableTest, VelocityClamping) {
  Movable m;
  // Set very high velocity
  m.setVelocity(Vector(1000, 0, 0));
  m.iterate();
  // Velocity should be clamped to MOVABLE_MAX_VELOCITY
  EXPECT_LE(m.getVelocity().getLengthVector(), MOVABLE_MAX_VELOCITY);
}

TEST(MovableTest, PositionUpdatesWithVelocity) {
  Movable m;
  m.setVelocity(Vector(100, 0, 0));
  Vector initialPos = m.getPosition();
  m.iterate();
  // Position should have increased in X
  EXPECT_GT(m.getPosition().getX(), initialPos.getX());
}

TEST(MovableTest, VelocityUpdatesWithAcceleration) {
  Movable m;
  m.setAcceleration(Vector(100, 0, 0));
  m.iterate();
  // Velocity should have increased due to acceleration
  EXPECT_GT(m.getVelocity().getX(), 0.0);
}

TEST(MovableTest, FrictionSlowsDown) {
  Movable m;
  m.setVelocity(Vector(10, 0, 0));

  for (int i = 0; i < 100; i++) {
    m.iterate();
  }

  // After many iterations with friction and no acceleration,
  // velocity should decrease significantly
  EXPECT_LT(m.getVelocity().getLengthVector(), 10.0);
}
