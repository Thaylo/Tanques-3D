/**
 * BattleRoyaleNN.cpp - Expanded neural network implementation
 */

#include "ai/BattleRoyaleNN.h"
#include <algorithm>
#include <cmath>

namespace AI {

BattleRoyaleNN::BattleRoyaleNN() {
  weights1.fill(0.0f);
  weights2.fill(0.0f);
  weights3.fill(0.0f);
  bias1.fill(0.0f);
  bias2.fill(0.0f);
  bias3.fill(0.0f);
}

void BattleRoyaleNN::randomize(std::mt19937 &rng) {
  // Xavier initialization for better gradient flow
  float scale1 = std::sqrt(2.0f / BR_INPUT_SIZE);
  float scale2 = std::sqrt(2.0f / BR_HIDDEN_SIZE);

  std::normal_distribution<float> dist1(0.0f, scale1);
  std::normal_distribution<float> dist2(0.0f, scale2);

  for (auto &w : weights1)
    w = dist1(rng);
  for (auto &w : weights2)
    w = dist2(rng);
  for (auto &w : weights3)
    w = dist2(rng);

  // Small bias initialization
  std::normal_distribution<float> biasDist(0.0f, 0.01f);
  for (auto &b : bias1)
    b = biasDist(rng);
  for (auto &b : bias2)
    b = biasDist(rng);
  for (auto &b : bias3)
    b = biasDist(rng);
}

// LeakyReLU activation (prevents dead neurons)
inline float leakyRelu(float x) { return x > 0 ? x : 0.01f * x; }

// Sigmoid for output normalization
inline float sigmoid(float x) {
  return 1.0f / (1.0f + std::exp(-std::clamp(x, -20.0f, 20.0f)));
}

std::array<float, BR_OUTPUT_SIZE>
BattleRoyaleNN::forward(const std::array<float, BR_INPUT_SIZE> &input) const {

  // Layer 1: Input (32) → Hidden1 (64)
  std::array<float, BR_HIDDEN_SIZE> hidden1;
  for (size_t j = 0; j < BR_HIDDEN_SIZE; ++j) {
    float sum = bias1[j];
    for (size_t i = 0; i < BR_INPUT_SIZE; ++i) {
      sum += input[i] * weights1[i * BR_HIDDEN_SIZE + j];
    }
    hidden1[j] = leakyRelu(sum);
  }

  // Layer 2: Hidden1 (64) → Hidden2 (64)
  std::array<float, BR_HIDDEN_SIZE> hidden2;
  for (size_t j = 0; j < BR_HIDDEN_SIZE; ++j) {
    float sum = bias2[j];
    for (size_t i = 0; i < BR_HIDDEN_SIZE; ++i) {
      sum += hidden1[i] * weights2[i * BR_HIDDEN_SIZE + j];
    }
    hidden2[j] = leakyRelu(sum);
  }

  // Layer 3: Hidden2 (64) → Output (4)
  std::array<float, BR_OUTPUT_SIZE> output;
  for (size_t j = 0; j < BR_OUTPUT_SIZE; ++j) {
    float sum = bias3[j];
    for (size_t i = 0; i < BR_HIDDEN_SIZE; ++i) {
      sum += hidden2[i] * weights3[i * BR_OUTPUT_SIZE + j];
    }
    // Output activations
    if (j == 0) {
      output[j] = std::tanh(sum); // Turn: -1 to 1
    } else {
      output[j] = sigmoid(sum); // Throttle, shoot, aggression: 0 to 1
    }
  }

  return output;
}

std::vector<float> BattleRoyaleNN::serialize() const {
  std::vector<float> data;
  data.reserve(BR_TOTAL_WEIGHTS);

  data.insert(data.end(), weights1.begin(), weights1.end());
  data.insert(data.end(), weights2.begin(), weights2.end());
  data.insert(data.end(), weights3.begin(), weights3.end());
  data.insert(data.end(), bias1.begin(), bias1.end());
  data.insert(data.end(), bias2.begin(), bias2.end());
  data.insert(data.end(), bias3.begin(), bias3.end());

  return data;
}

void BattleRoyaleNN::deserialize(const std::vector<float> &data) {
  if (data.size() < BR_TOTAL_WEIGHTS)
    return;

  size_t idx = 0;
  std::copy_n(data.begin() + idx, weights1.size(), weights1.begin());
  idx += weights1.size();
  std::copy_n(data.begin() + idx, weights2.size(), weights2.begin());
  idx += weights2.size();
  std::copy_n(data.begin() + idx, weights3.size(), weights3.begin());
  idx += weights3.size();
  std::copy_n(data.begin() + idx, bias1.size(), bias1.begin());
  idx += bias1.size();
  std::copy_n(data.begin() + idx, bias2.size(), bias2.begin());
  idx += bias2.size();
  std::copy_n(data.begin() + idx, bias3.size(), bias3.begin());
}

BattleRoyaleNN BattleRoyaleNN::crossover(const BattleRoyaleNN &a,
                                         const BattleRoyaleNN &b,
                                         std::mt19937 &rng) {
  BattleRoyaleNN child;
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  // Blend crossover (weighted average based on fitness)
  float totalFit = std::max(0.1f, a.fitness + b.fitness);
  float aWeight = a.fitness / totalFit;
  float bWeight = b.fitness / totalFit;

  for (size_t i = 0; i < a.weights1.size(); ++i) {
    if (dist(rng) < 0.5f) {
      // Blend
      child.weights1[i] = aWeight * a.weights1[i] + bWeight * b.weights1[i];
    } else {
      // Uniform crossover
      child.weights1[i] = (dist(rng) < 0.5f) ? a.weights1[i] : b.weights1[i];
    }
  }

  for (size_t i = 0; i < a.weights2.size(); ++i) {
    if (dist(rng) < 0.5f) {
      child.weights2[i] = aWeight * a.weights2[i] + bWeight * b.weights2[i];
    } else {
      child.weights2[i] = (dist(rng) < 0.5f) ? a.weights2[i] : b.weights2[i];
    }
  }

  for (size_t i = 0; i < a.weights3.size(); ++i) {
    if (dist(rng) < 0.5f) {
      child.weights3[i] = aWeight * a.weights3[i] + bWeight * b.weights3[i];
    } else {
      child.weights3[i] = (dist(rng) < 0.5f) ? a.weights3[i] : b.weights3[i];
    }
  }

  // Crossover biases
  for (size_t i = 0; i < a.bias1.size(); ++i) {
    child.bias1[i] = (dist(rng) < 0.5f) ? a.bias1[i] : b.bias1[i];
  }
  for (size_t i = 0; i < a.bias2.size(); ++i) {
    child.bias2[i] = (dist(rng) < 0.5f) ? a.bias2[i] : b.bias2[i];
  }
  for (size_t i = 0; i < a.bias3.size(); ++i) {
    child.bias3[i] = (dist(rng) < 0.5f) ? a.bias3[i] : b.bias3[i];
  }

  return child;
}

void BattleRoyaleNN::mutate(float rate, float strength, std::mt19937 &rng) {
  std::uniform_real_distribution<float> chance(0.0f, 1.0f);
  std::normal_distribution<float> noise(0.0f, strength);

  for (auto &w : weights1)
    if (chance(rng) < rate)
      w += noise(rng);
  for (auto &w : weights2)
    if (chance(rng) < rate)
      w += noise(rng);
  for (auto &w : weights3)
    if (chance(rng) < rate)
      w += noise(rng);
  for (auto &b : bias1)
    if (chance(rng) < rate)
      b += noise(rng);
  for (auto &b : bias2)
    if (chance(rng) < rate)
      b += noise(rng);
  for (auto &b : bias3)
    if (chance(rng) < rate)
      b += noise(rng);
}

float BattleRoyaleNN::geneticDistance(const BattleRoyaleNN &other) const {
  float dist = 0.0f;

  // Sample weights to estimate distance (faster than full comparison)
  for (size_t i = 0; i < weights1.size(); i += 10) {
    dist += std::abs(weights1[i] - other.weights1[i]);
  }
  for (size_t i = 0; i < weights2.size(); i += 10) {
    dist += std::abs(weights2[i] - other.weights2[i]);
  }

  return dist;
}

} // namespace AI
