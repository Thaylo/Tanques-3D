/**
 * test_ai_controller.mm - AI Controller integration tests
 *
 * Tests Core ML Neural Engine AI on Apple Silicon.
 */

#include "ai/AIController.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

namespace {

TEST(AIController, InitializationTest) {
  AI::AIController controller;

  // Should not throw on Apple Silicon
  EXPECT_NO_THROW(controller.initialize());

  std::cout << "AI Compute: " << controller.getComputeDevice() << std::endl;
  EXPECT_TRUE(controller.isNeuralEngineAvailable());

  controller.shutdown();
}

TEST(AIController, PredictDecision) {
  AI::AIController controller;
  controller.initialize();

  // Test prediction with sample input
  AI::AIInput input = {.distanceToTarget = 20.0f,
                       .angleToTarget = 0.5f, // ~30 degrees to the right
                       .targetVelocityX = 0.0f,
                       .targetVelocityY = 0.0f,
                       .myVelocityX = 1.0f,
                       .myVelocityY = 0.0f,
                       .myHealth = 100.0f,
                       .targetHealth = 100.0f};

  AI::AIDecision decision = controller.predict(input);

  // Should turn right (positive) since target is to the right
  EXPECT_GT(decision.turnDirection, 0.0f);

  // Should move since distance > 15
  EXPECT_GT(decision.throttle, 0.0f);

  // Should NOT shoot since not aimed (angle > PI/15)
  EXPECT_NEAR(decision.shouldShoot, 0.0f, 0.1f);

  std::cout << "Decision: turn=" << decision.turnDirection
            << " throttle=" << decision.throttle
            << " shoot=" << decision.shouldShoot << std::endl;

  controller.shutdown();
}

TEST(AIController, BatchPrediction) {
  AI::AIController controller;
  controller.initialize();

  // Test batch prediction for multiple enemies
  std::vector<AI::AIInput> inputs = {
      {10.0f, 0.0f, 0, 0, 0, 0, 100, 100}, // Close, aimed
      {50.0f, 1.0f, 0, 0, 0, 0, 100, 100}, // Far, off-target
      {5.0f, 0.1f, 0, 0, 0, 0, 100, 100}   // Very close, almost aimed
  };

  auto decisions = controller.predictBatch(inputs);

  EXPECT_EQ(decisions.size(), 3);

  // First enemy: close and aimed - should shoot
  EXPECT_NEAR(decisions[0].shouldShoot, 1.0f, 0.1f);

  // Second enemy: far - should move at full throttle
  EXPECT_NEAR(decisions[1].throttle, 1.0f, 0.1f);

  // Third enemy: very close - should stop
  EXPECT_NEAR(decisions[2].throttle, 0.0f, 0.1f);

  controller.shutdown();
}

} // namespace
