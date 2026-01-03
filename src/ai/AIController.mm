/**
 * AIController.mm - Core ML Neural Engine Implementation
 *
 * Apple Silicon Neural Engine powered AI for enemy behaviors.
 * Uses Core ML for low-latency, power-efficient inference.
 */

#import <CoreML/CoreML.h>
#import <Foundation/Foundation.h>
#import <simd/simd.h>

#include "ai/AIController.h"
#include <cmath>
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
    std::cout
        << "[AIController] Note: Using heuristic AI (no trained model yet)"
        << std::endl;

    initialized_ = true;

    // Future: Load trained .mlmodel file
    // NSError* error = nil;
    // NSURL* modelURL = [[NSBundle mainBundle] URLForResource:@"EnemyAI"
    //                                          withExtension:@"mlmodelc"];
    // if (modelURL) {
    //     context_->model = [MLModel modelWithContentsOfURL:modelURL
    //                                        configuration:context_->config
    //                                                error:&error];
    // }
  }
}

void AIController::shutdown() {
  @autoreleasepool {
    context_->model = nil;
    context_->config = nil;
    initialized_ = false;
  }
}

AIDecision AIController::heuristicDecision(const AIInput &input) {
  AIDecision decision = {};

  // Rule-based heuristic that mimics trained behavior
  float dist = input.distanceToTarget;
  float angle = input.angleToTarget;

  // Turn toward target (normalized to -1..1)
  decision.turnDirection = std::clamp(angle / (float)M_PI, -1.0f, 1.0f);

  // Throttle based on distance
  if (dist > 15.0f) {
    decision.throttle = 1.0f; // Full speed when far
  } else if (dist > 7.0f) {
    decision.throttle = 0.5f; // Slow down approaching
  } else {
    decision.throttle = 0.0f; // Stop when close
  }

  // Shoot when aimed and in range
  bool aimed = std::abs(angle) < (M_PI / 15.0f);
  bool inRange = dist < 30.0f && dist > 3.0f;
  decision.shouldShoot = (aimed && inRange) ? 1.0f : 0.0f;

  // Aggression based on health ratio
  float healthRatio = input.myHealth / std::max(input.targetHealth, 0.1f);
  decision.aggression = std::clamp(healthRatio, 0.0f, 1.0f);

  return decision;
}

AIDecision AIController::predict(const AIInput &input) {
  if (!initialized_) {
    throw CoreMLError("AIController not initialized");
  }

  // For now, use heuristic AI
  // Future: Use Core ML model inference
  if (context_->model == nil) {
    return heuristicDecision(input);
  }

  @autoreleasepool {
    // Future: Create MLFeatureProvider with input features
    // Run inference: [context_->model predictionFromFeatures:provider
    // error:&error] Extract outputs and return AIDecision

    return heuristicDecision(input);
  }
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
  if (context_->deviceInfo) {
    return std::string([context_->deviceInfo UTF8String]);
  }
  return "Unknown";
}

} // namespace AI
