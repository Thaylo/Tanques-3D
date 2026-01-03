/**
 * AIController.h - Core ML Neural Engine AI Controller
 *
 * Runs AI inference on Apple Neural Engine for enemy behaviors.
 * Apple Silicon ONLY - no fallbacks.
 */

#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ai/NeuralNetwork.h"

namespace AI {

// Forward declarations
struct CoreMLContext;

/**
 * AI decision output from Neural Engine
 */
struct AIDecision {
  float turnDirection; // -1 to 1 (left to right)
  float throttle;      // 0 to 1 (speed)
  float shouldShoot;   // 0 to 1 (fire probability)
  float aggression;    // 0 to 1 (attack vs evade)
};

/**
 * Input features for AI model
 */
struct AIInput {
  float distanceToTarget;
  float angleToTarget;
  float targetVelocityX;
  float targetVelocityY;
  float myVelocityX;
  float myVelocityY;
  float myHealth;
  float targetHealth;
};

/**
 * CoreMLError - Thrown when Core ML operations fail
 */
class CoreMLError : public std::runtime_error {
public:
  explicit CoreMLError(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * AIController - Neural Engine powered AI
 *
 * Uses trained neural network for intelligent enemy decisions.
 */
class AIController {
public:
  AIController();
  ~AIController();

  // Initialize Core ML runtime - throws on failure
  void initialize();

  // Load trained weights from file
  bool loadWeights(const std::string &path);

  // Shutdown Core ML
  void shutdown();

  // Run AI inference
  AIDecision predict(const AIInput &input);

  // Batch prediction for multiple enemies
  std::vector<AIDecision> predictBatch(const std::vector<AIInput> &inputs);

  // Check if Neural Engine is available
  bool isNeuralEngineAvailable() const;

  // Check if trained model is loaded
  bool isTrainedModelLoaded() const { return trainedModelLoaded_; }

  // Get device info
  std::string getComputeDevice() const;

private:
  std::unique_ptr<CoreMLContext> context_;
  NeuralNetwork trainedNetwork_;
  bool initialized_ = false;
  bool trainedModelLoaded_ = false;

  // Fallback heuristic AI when model not available
  AIDecision heuristicDecision(const AIInput &input);

  // Neural network inference
  AIDecision neuralNetworkDecision(const AIInput &input);
};

} // namespace AI

#endif // AI_CONTROLLER_H
