/**
 * NeuralNetwork.cpp - Neural network implementation
 */

#include "ai/NeuralNetwork.h"
#include <algorithm>
#include <cmath>

namespace AI {

NeuralNetwork::NeuralNetwork() {
  // Zero initialize all weights and biases
  weights1.fill(0.0f);
  weights2.fill(0.0f);
  weights3.fill(0.0f);
  bias1.fill(0.0f);
  bias2.fill(0.0f);
  bias3.fill(0.0f);
}

void NeuralNetwork::randomize(std::mt19937 &rng) {
  std::normal_distribution<float> dist(0.0f, 0.5f);

  for (auto &w : weights1)
    w = dist(rng);
  for (auto &w : weights2)
    w = dist(rng);
  for (auto &w : weights3)
    w = dist(rng);
  for (auto &b : bias1)
    b = dist(rng) * 0.1f;
  for (auto &b : bias2)
    b = dist(rng) * 0.1f;
  for (auto &b : bias3)
    b = dist(rng) * 0.1f;
}

// ReLU activation
inline float relu(float x) { return std::max(0.0f, x); }

// Sigmoid for output normalization
inline float sigmoid(float x) { return 1.0f / (1.0f + std::exp(-x)); }

std::array<float, OUTPUT_SIZE>
NeuralNetwork::forward(const std::array<float, INPUT_SIZE> &input) const {

  // Layer 1: Input (8) → Hidden1 (16)
  std::array<float, HIDDEN_SIZE> hidden1;
  for (size_t j = 0; j < HIDDEN_SIZE; ++j) {
    float sum = bias1[j];
    for (size_t i = 0; i < INPUT_SIZE; ++i) {
      sum += input[i] * weights1[i * HIDDEN_SIZE + j];
    }
    hidden1[j] = relu(sum);
  }

  // Layer 2: Hidden1 (16) → Hidden2 (16)
  std::array<float, HIDDEN_SIZE> hidden2;
  for (size_t j = 0; j < HIDDEN_SIZE; ++j) {
    float sum = bias2[j];
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
      sum += hidden1[i] * weights2[i * HIDDEN_SIZE + j];
    }
    hidden2[j] = relu(sum);
  }

  // Layer 3: Hidden2 (16) → Output (4)
  std::array<float, OUTPUT_SIZE> output;
  for (size_t j = 0; j < OUTPUT_SIZE; ++j) {
    float sum = bias3[j];
    for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
      sum += hidden2[i] * weights3[i * OUTPUT_SIZE + j];
    }
    // Normalize outputs to useful ranges
    if (j == 0) {
      output[j] = std::tanh(sum); // Turn: -1 to 1
    } else {
      output[j] = sigmoid(sum); // Throttle, shoot, aggression: 0 to 1
    }
  }

  return output;
}

std::vector<float> NeuralNetwork::serialize() const {
  std::vector<float> data;
  data.reserve(TOTAL_WEIGHTS);

  data.insert(data.end(), weights1.begin(), weights1.end());
  data.insert(data.end(), weights2.begin(), weights2.end());
  data.insert(data.end(), weights3.begin(), weights3.end());
  data.insert(data.end(), bias1.begin(), bias1.end());
  data.insert(data.end(), bias2.begin(), bias2.end());
  data.insert(data.end(), bias3.begin(), bias3.end());

  return data;
}

void NeuralNetwork::deserialize(const std::vector<float> &data) {
  if (data.size() < TOTAL_WEIGHTS)
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

NeuralNetwork NeuralNetwork::crossover(const NeuralNetwork &a,
                                       const NeuralNetwork &b,
                                       std::mt19937 &rng) {
  NeuralNetwork child;
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  // Uniform crossover for weights
  for (size_t i = 0; i < a.weights1.size(); ++i) {
    child.weights1[i] = (dist(rng) < 0.5f) ? a.weights1[i] : b.weights1[i];
  }
  for (size_t i = 0; i < a.weights2.size(); ++i) {
    child.weights2[i] = (dist(rng) < 0.5f) ? a.weights2[i] : b.weights2[i];
  }
  for (size_t i = 0; i < a.weights3.size(); ++i) {
    child.weights3[i] = (dist(rng) < 0.5f) ? a.weights3[i] : b.weights3[i];
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

void NeuralNetwork::mutate(float mutationRate, float mutationStrength,
                           std::mt19937 &rng) {
  std::uniform_real_distribution<float> chance(0.0f, 1.0f);
  std::normal_distribution<float> noise(0.0f, mutationStrength);

  for (auto &w : weights1)
    if (chance(rng) < mutationRate)
      w += noise(rng);
  for (auto &w : weights2)
    if (chance(rng) < mutationRate)
      w += noise(rng);
  for (auto &w : weights3)
    if (chance(rng) < mutationRate)
      w += noise(rng);
  for (auto &b : bias1)
    if (chance(rng) < mutationRate)
      b += noise(rng);
  for (auto &b : bias2)
    if (chance(rng) < mutationRate)
      b += noise(rng);
  for (auto &b : bias3)
    if (chance(rng) < mutationRate)
      b += noise(rng);
}

} // namespace AI
