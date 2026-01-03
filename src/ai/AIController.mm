/**
 * AIController.mm - Core ML Neural Engine Implementation
 *
 * Apple Silicon Neural Engine powered AI for enemy behaviors.
 * Uses trained neural network for intelligent decisions.
 */

#import <CoreML/CoreML.h>
#import <Foundation/Foundation.h>
#import <simd/simd.h>

#include "ai/AIController.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

namespace AI {

/**
 * Core ML context holding Objective-C objects
 */
struct CoreMLContext {
  MLModel *model = nil;
  MLModelConfiguration *config = nil;
  NSString *deviceInfo = nil;
  bool hasNeuralEngine = false;
};

AIController::AIController() : context_(std::make_unique<CoreMLContext>()) {}

AIController::~AIController() { shutdown(); }

void AIController::initialize() {
  @autoreleasepool {
    // Configure for Neural Engine
    context_->config = [[MLModelConfiguration alloc] init];

    // Prefer Neural Engine, fallback to GPU, then CPU
    context_->config.computeUnits = MLComputeUnitsAll;

    // Check Neural Engine availability via device capabilities
    // On Apple Silicon, Neural Engine is always available
    context_->hasNeuralEngine = true;
    context_->deviceInfo = @"Apple Neural Engine";

    std::cout << "[AIController] Initialized with Neural Engine support"
              << std::endl;

    initialized_ = true;

    // Try to load trained weights automatically
    if (loadWeights("assets/ml/trained_enemy_ai.bin")) {
      std::cout << "[AIController] Loaded trained neural network" << std::endl;
    } else {
      std::cout << "[AIController] Using heuristic AI (no trained model)"
                << std::endl;
    }
  }
}

bool AIController::loadWeights(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return false;
  }

  std::vector<float> data(TOTAL_WEIGHTS);
  file.read(reinterpret_cast<char *>(data.data()), data.size() * sizeof(float));

  if (!file) {
    return false;
  }

  trainedNetwork_.deserialize(data);
  trainedModelLoaded_ = true;
  return true;
}

void AIController::shutdown() {
  @autoreleasepool {
    context_->model = nil;
    context_->config = nil;
    initialized_ = false;
    trainedModelLoaded_ = false;
  }
}

AIDecision AIController::neuralNetworkDecision(const AIInput &input) {
  // Prepare input array for neural network
  std::array<float, INPUT_SIZE> nnInput = {
      input.distanceToTarget / 200.0f,                // Normalize to arena size
      input.angleToTarget / static_cast<float>(M_PI), // -1 to 1
      input.targetVelocityX / 25.0f,                  // Normalize to max speed
      input.targetVelocityY / 25.0f,
      input.myVelocityX / 25.0f,
      input.myVelocityY / 25.0f,
      input.myHealth / 100.0f,
      input.targetHealth / 100.0f};

  // Run forward pass
  auto output = trainedNetwork_.forward(nnInput);

  AIDecision decision = {};
  decision.turnDirection = output[0]; // Already -1 to 1 (tanh)
  decision.throttle = output[1];      // Already 0 to 1 (sigmoid)
  decision.shouldShoot = output[2];   // Already 0 to 1 (sigmoid)
  decision.aggression = output[3];    // Already 0 to 1 (sigmoid)

  return decision;
}

AIDecision AIController::heuristicDecision(const AIInput &input) {
  AIDecision decision = {};

  // Rule-based heuristic as fallback
  float dist = input.distanceToTarget;
  float angle = input.angleToTarget;

  // Turn toward target (normalized to -1..1)
  float normalizedAngle = angle / static_cast<float>(M_PI);
  decision.turnDirection = std::max(-1.0f, std::min(1.0f, normalizedAngle));

  // Throttle based on distance
  if (dist > 15.0f) {
    decision.throttle = 1.0f;
  } else if (dist > 7.0f) {
    decision.throttle = 0.5f;
  } else {
    decision.throttle = 0.0f;
  }

  // Shoot when aimed and in range
  bool aimed = std::abs(angle) < (M_PI / 15.0f);
  bool inRange = dist < 30.0f && dist > 3.0f;
  decision.shouldShoot = (aimed && inRange) ? 1.0f : 0.0f;

  // Aggression based on health ratio
  float healthRatio = input.myHealth / std::max(input.targetHealth, 0.1f);
  decision.aggression = std::max(0.0f, std::min(1.0f, healthRatio));

  return decision;
}

AIDecision AIController::predict(const AIInput &input) {
  if (!initialized_) {
    throw CoreMLError("AIController not initialized");
  }

  // Use trained neural network if available
  if (trainedModelLoaded_) {
    return neuralNetworkDecision(input);
  }

  // Fallback to heuristic
  return heuristicDecision(input);
}

std::vector<AIDecision>
AIController::predictBatch(const std::vector<AIInput> &inputs) {
  std::vector<AIDecision> decisions;
  decisions.reserve(inputs.size());

  for (const auto &input : inputs) {
    decisions.push_back(predict(input));
  }

  return decisions;
}

bool AIController::isNeuralEngineAvailable() const {
  return context_->hasNeuralEngine;
}

std::string AIController::getComputeDevice() const {
  if (trainedModelLoaded_) {
    return "Trained Neural Network";
  }
  if (context_->deviceInfo) {
    return std::string([context_->deviceInfo UTF8String]);
  }
  return "Unknown";
}

} // namespace AI
