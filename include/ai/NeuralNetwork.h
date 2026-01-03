/**
 * NeuralNetwork.h - Compact neural network for enemy AI
 *
 * Small architecture optimized for Apple Neural Engine:
 * - Input: 8 features
 * - Hidden: 2×16 neurons (ReLU)
 * - Output: 4 decisions
 */

#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <array>
#include <cstdint>
#include <random>
#include <vector>

namespace AI {

// Network dimensions
constexpr size_t INPUT_SIZE = 8;
constexpr size_t HIDDEN_SIZE = 16;
constexpr size_t OUTPUT_SIZE = 4;

// Total weight count for serialization
constexpr size_t TOTAL_WEIGHTS = INPUT_SIZE * HIDDEN_SIZE +  // Layer 1
                                 HIDDEN_SIZE * HIDDEN_SIZE + // Layer 2
                                 HIDDEN_SIZE * OUTPUT_SIZE + // Layer 3
                                 HIDDEN_SIZE + HIDDEN_SIZE +
                                 OUTPUT_SIZE; // Biases

/**
 * NeuralNetwork - Feedforward network for AI decisions
 */
class NeuralNetwork {
public:
  // Weights
  std::array<float, INPUT_SIZE * HIDDEN_SIZE> weights1;  // Input→Hidden1
  std::array<float, HIDDEN_SIZE * HIDDEN_SIZE> weights2; // Hidden1→Hidden2
  std::array<float, HIDDEN_SIZE * OUTPUT_SIZE> weights3; // Hidden2→Output

  // Biases
  std::array<float, HIDDEN_SIZE> bias1;
  std::array<float, HIDDEN_SIZE> bias2;
  std::array<float, OUTPUT_SIZE> bias3;

  NeuralNetwork();

  // Initialize with random weights
  void randomize(std::mt19937 &rng);

  // Forward pass: input[8] → output[4]
  std::array<float, OUTPUT_SIZE>
  forward(const std::array<float, INPUT_SIZE> &input) const;

  // Serialization
  std::vector<float> serialize() const;
  void deserialize(const std::vector<float> &data);

  // Genetic operators
  static NeuralNetwork crossover(const NeuralNetwork &a, const NeuralNetwork &b,
                                 std::mt19937 &rng);
  void mutate(float mutationRate, float mutationStrength, std::mt19937 &rng);

  // Fitness (set during training)
  float fitness = 0.0f;
};

} // namespace AI

#endif // NEURAL_NETWORK_H
