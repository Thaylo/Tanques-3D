/**
 * BattleRoyaleNN.h - Expanded neural network for Battle Royale AI
 *
 * 32 inputs → 64 → 64 → 4 outputs
 * Uses LeakyReLU to prevent dead neurons
 */

#ifndef BATTLE_ROYALE_NN_H
#define BATTLE_ROYALE_NN_H

#include <array>
#include <cmath>
#include <random>
#include <vector>

namespace AI {

// Network dimensions
constexpr size_t BR_INPUT_SIZE = 32;
constexpr size_t BR_HIDDEN_SIZE = 64;
constexpr size_t BR_OUTPUT_SIZE = 4;

// Total weights for serialization
constexpr size_t BR_TOTAL_WEIGHTS = BR_INPUT_SIZE * BR_HIDDEN_SIZE +  // Layer 1
                                    BR_HIDDEN_SIZE * BR_HIDDEN_SIZE + // Layer 2
                                    BR_HIDDEN_SIZE * BR_OUTPUT_SIZE + // Layer 3
                                    BR_HIDDEN_SIZE + BR_HIDDEN_SIZE +
                                    BR_OUTPUT_SIZE; // Biases

/**
 * Input indices for easier access
 */
namespace BRInput {
// Closest enemy 1 (6 values)
constexpr int ENEMY1_DIST = 0;
constexpr int ENEMY1_ANGLE = 1;
constexpr int ENEMY1_VX = 2;
constexpr int ENEMY1_VY = 3;
constexpr int ENEMY1_HEALTH = 4;
constexpr int ENEMY1_RELOAD = 5;

// Closest enemy 2 (6 values)
constexpr int ENEMY2_DIST = 6;
constexpr int ENEMY2_ANGLE = 7;
constexpr int ENEMY2_VX = 8;
constexpr int ENEMY2_VY = 9;
constexpr int ENEMY2_HEALTH = 10;
constexpr int ENEMY2_RELOAD = 11;

// Closest enemy 3 (6 values)
constexpr int ENEMY3_DIST = 12;
constexpr int ENEMY3_ANGLE = 13;
constexpr int ENEMY3_VX = 14;
constexpr int ENEMY3_VY = 15;
constexpr int ENEMY3_HEALTH = 16;
constexpr int ENEMY3_RELOAD = 17;

// Self state
constexpr int SELF_AZIMUTH = 18; // CCW from +X axis
constexpr int SELF_SPEED = 19;   // Velocity magnitude
constexpr int SELF_HEADING = 20; // Velocity direction
constexpr int SELF_HEALTH = 21;
constexpr int SELF_RELOAD = 22; // 0=ready, 1=reloading

// Safe zone
constexpr int ZONE_DIST = 23;   // Distance to edge (normalized safety margin)
constexpr int ZONE_DIR = 24;    // Direction to center (self-centered)
constexpr int ZONE_RADIUS = 25; // Current radius (normalized)
constexpr int ZONE_TIMER = 26;  // Time pressure (0=safe, 1=shrinking soon)

// Zone center position (self-centered, rotates with agent)
constexpr int ZONE_CENTER_X = 27; // Local X to zone center
constexpr int ZONE_CENTER_Y = 28; // Local Y to zone center

// Reserved for future
constexpr int RESERVED1 = 29;
constexpr int RESERVED2 = 30;
constexpr int RESERVED3 = 31;
} // namespace BRInput

/**
 * BattleRoyaleNN - Larger neural network for complex Battle Royale AI
 */
class BattleRoyaleNN {
public:
  // Weights
  std::array<float, BR_INPUT_SIZE * BR_HIDDEN_SIZE> weights1;
  std::array<float, BR_HIDDEN_SIZE * BR_HIDDEN_SIZE> weights2;
  std::array<float, BR_HIDDEN_SIZE * BR_OUTPUT_SIZE> weights3;

  // Biases
  std::array<float, BR_HIDDEN_SIZE> bias1;
  std::array<float, BR_HIDDEN_SIZE> bias2;
  std::array<float, BR_OUTPUT_SIZE> bias3;

  // Evolution tracking
  float fitness = 0.0f;
  int speciesId = 0;
  int age = 0;

  BattleRoyaleNN();

  void randomize(std::mt19937 &rng);

  // Forward pass with LeakyReLU
  std::array<float, BR_OUTPUT_SIZE>
  forward(const std::array<float, BR_INPUT_SIZE> &input) const;

  // Serialization
  std::vector<float> serialize() const;
  void deserialize(const std::vector<float> &data);

  // Genetic operators
  static BattleRoyaleNN crossover(const BattleRoyaleNN &a,
                                  const BattleRoyaleNN &b, std::mt19937 &rng);
  void mutate(float rate, float strength, std::mt19937 &rng);

  // Distance metric for speciation
  float geneticDistance(const BattleRoyaleNN &other) const;
};

} // namespace AI

#endif // BATTLE_ROYALE_NN_H
