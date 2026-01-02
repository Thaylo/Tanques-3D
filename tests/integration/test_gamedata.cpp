/**
 * test_gamedata.cpp - Integration tests for GameData
 *
 * Note: These tests focus on non-OpenGL logic since full GL context
 * is not available in unit test environment.
 */

#include "core/Constants.h"
#include "core/Vector.h"
#include <gtest/gtest.h>

// Test constants are properly defined
TEST(GameDataIntegration, ConstantsAreDefined) {
  EXPECT_EQ(PLAYER_ID, 642);
  EXPECT_GT(MOVABLE_MAX_VELOCITY, 0);
  EXPECT_GT(MOVABLE_MAX_ACCELERATION, 0);
  EXPECT_GT(TIME_STEP, 0);
  EXPECT_GT(RELOAD_ROUNDS, 0);
}

// Test that collision detection math works
TEST(GameDataIntegration, CollisionDistanceCalculation) {
  Vector pos1(0, 0, 0);
  Vector pos2(0.3, 0.3, 0);

  double dist = (pos1 - pos2).getLengthVector();

  // Distance should be sqrt(0.18) ≈ 0.424
  EXPECT_LT(dist, 0.5); // Within collision range
}

TEST(GameDataIntegration, NoCollisionDistanceCalculation) {
  Vector pos1(0, 0, 0);
  Vector pos2(5, 5, 0);

  double dist = (pos1 - pos2).getLengthVector();

  // Distance should be sqrt(50) ≈ 7.07
  EXPECT_GT(dist, 0.5); // Outside collision range
}

// Test projectile max distance logic
TEST(GameDataIntegration, ProjectileMaxDistanceCheck) {
  Vector initialPos(0, 0, 0);
  Vector currentPos(25, 25, 0); // sqrt(1250) ≈ 35.4

  double traveledDistance = (currentPos - initialPos).getLengthVector();

  // Should still be within range (< 40)
  EXPECT_LT(traveledDistance, 40);
}

TEST(GameDataIntegration, ProjectileBeyondMaxDistance) {
  Vector initialPos(0, 0, 0);
  Vector currentPos(30, 30, 0);

  double traveledDistance = (currentPos - initialPos).getLengthVector();

  // sqrt(1800) ≈ 42.4, should exceed 40
  EXPECT_GT(traveledDistance, 40);
}

// Test AI targeting angle calculation
TEST(GameDataIntegration, AITargetingAngleCalculation) {
  Vector enemyPos(0, 0, 0);
  Vector playerPos(10, 0, 0);
  Vector enemyDir(1, 0, 0);

  Vector toTarget = playerPos - enemyPos;
  double sine = toTarget.crossProduct(enemyDir).getLengthVector();
  double cosine = toTarget.dotProduct(enemyDir);
  double theta = atan2(sine, cosine);

  // Enemy is facing player directly, angle should be ~0
  EXPECT_NEAR(theta, 0.0, 0.01);
}

TEST(GameDataIntegration, AITargetingAngledApproach) {
  Vector enemyPos(0, 0, 0);
  Vector playerPos(0, 10, 0); // Player is to the side
  Vector enemyDir(1, 0, 0);   // Enemy facing forward

  Vector toTarget = playerPos - enemyPos;
  double sine = toTarget.crossProduct(enemyDir).getLengthVector();
  double cosine = toTarget.dotProduct(enemyDir);
  double theta = atan2(sine, cosine);

  // Enemy needs to turn ~90 degrees
  EXPECT_NEAR(fabs(theta), M_PI / 2, 0.01);
}
